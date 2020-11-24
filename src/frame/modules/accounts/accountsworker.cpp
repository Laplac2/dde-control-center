/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "accountsworker.h"
#include "user.h"
#include "window/utils.h"
#include "widgets/utils.h"

#include <QFileDialog>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QStandardPaths>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QGSettings>

#include <pwd.h>
#include <unistd.h>
#include <libintl.h>
#include <random>
#include <crypt.h>

using namespace dcc::accounts;
using namespace DCC_NAMESPACE;

const QString AccountsService("com.deepin.daemon.Accounts");
const QString DisplayManagerService("org.freedesktop.DisplayManager");

const QString AutoLoginVisable = "auto-login-visable";
const QString NoPasswordVisable = "nopasswd-login-visable";

AccountsWorker::AccountsWorker(UserModel *userList, QObject *parent)
    : QObject(parent)
    , m_accountsInter(new Accounts(AccountsService, "/com/deepin/daemon/Accounts", QDBusConnection::systemBus(), this))
#ifdef DCC_ENABLE_ADDOMAIN
    , m_notifyInter(new Notifications("org.freedesktop.Notifications", "/org/freedesktop/Notifications", QDBusConnection::sessionBus(), this))
#endif
    , m_dmInter(new DisplayManager(DisplayManagerService, "/org/freedesktop/DisplayManager", QDBusConnection::systemBus(), this))
    , m_userModel(userList)
{
    struct passwd *pws;
    pws = getpwuid(getuid());
    m_currentUserName = QString(pws->pw_name);

    connect(m_accountsInter, &Accounts::UserListChanged, this, &AccountsWorker::onUserListChanged, Qt::QueuedConnection);
    connect(m_accountsInter, &Accounts::UserAdded, this, &AccountsWorker::addUser, Qt::QueuedConnection);
    connect(m_accountsInter, &Accounts::UserDeleted, this, &AccountsWorker::removeUser, Qt::QueuedConnection);

    connect(m_dmInter, &DisplayManager::SessionsChanged, this, &AccountsWorker::updateUserOnlineStatus);

    m_accountsInter->setSync(false);
    m_dmInter->setSync(false);
#ifdef DCC_ENABLE_ADDOMAIN
    m_notifyInter->setSync(false);
#endif
    onUserListChanged(m_accountsInter->userList());
    updateUserOnlineStatus(m_dmInter->sessions());
    getAllGroups();
    getPresetGroups();

    // 非服务器版本关联gsetting自动登陆/无密码登陆配置
    if (!IsServerSystem) {
        QGSettings *gsetting = new QGSettings("com.deepin.dde.control-center", QByteArray(), this);

        m_userModel->setAutoLoginVisable(gsetting->get(AutoLoginVisable).toBool());
        m_userModel->setNoPassWordLoginVisable(gsetting->get(NoPasswordVisable).toBool());
        connect(gsetting, &QGSettings::changed, m_userModel, [=](const QString &key){
            if (key == "autoLoginVisable") {
                m_userModel->setAutoLoginVisable(gsetting->get(AutoLoginVisable).toBool());
            } else if (key == "nopasswdLoginVisable") {
                m_userModel->setNoPassWordLoginVisable(gsetting->get(NoPasswordVisable).toBool());
            }
        });
    } else {
        m_userModel->setAutoLoginVisable(true);
        m_userModel->setNoPassWordLoginVisable(false);
    }


    bool bShowCreateUser = valueByQSettings<bool>(DCC_CONFIG_FILES, "", "showCreateUser", true);
    m_userModel->setCreateUserValid(bShowCreateUser);
}

void AccountsWorker::getAllGroups()
{
    QDBusPendingReply<QStringList> reply = m_accountsInter->GetGroups();
    QDBusPendingCallWatcher *groupResult = new QDBusPendingCallWatcher(reply, this);
    connect(groupResult, &QDBusPendingCallWatcher::finished, this, &AccountsWorker::getAllGroupsResult);
}

void AccountsWorker::getAllGroupsResult(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QStringList> reply = *watch;
    if (!watch->isError()) {
        m_userModel->setAllGroups(reply.value());
    } else {
        qDebug() << "getAllGroupsResult error." << watch->error();
    }
    watch->deleteLater();
}

void AccountsWorker::getPresetGroups()
{
    int userType = DCC_NAMESPACE::IsServerSystem ? 0 : 1;
    QDBusPendingReply<QStringList> reply = m_accountsInter->GetPresetGroups(userType);
    QDBusPendingCallWatcher *presetGroupsResult = new QDBusPendingCallWatcher(reply, this);
    connect(presetGroupsResult, &QDBusPendingCallWatcher::finished, this, &AccountsWorker::getPresetGroupsResult);
}

void AccountsWorker::getPresetGroupsResult(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QStringList> reply = *watch;
    if (!watch->isError()) {
        m_userModel->setPresetGroups(reply.value());
    } else {
        qDebug() << "getPresetGroupsResult error." << watch->error();
    }
    watch->deleteLater();
}

void AccountsWorker::setGroups(User *user, const QStringList &usrGroups)
{
    AccountsUser *userInter = m_userInters[user];
    Q_ASSERT(userInter);

    userInter->SetGroups(usrGroups);
}

void AccountsWorker::active()
{
    for (auto it(m_userInters.cbegin()); it != m_userInters.cend(); ++it) {
        it.key()->setName(it.value()->userName());
        it.key()->setAutoLogin(it.value()->automaticLogin());
        it.key()->setAvatars(it.value()->iconList());
        it.key()->setGroups(it.value()->groups());
        it.key()->setCurrentAvatar(it.value()->iconFile());
        it.key()->setCreatedTime(it.value()->createdTime());
    }
}

QString AccountsWorker::getCurrentUserName()
{
    return m_currentUserName;
}

void AccountsWorker::randomUserIcon(User *user)
{
    QDBusPendingCall call = m_accountsInter->RandUserIcon();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [=] {
        if (!call.isError()) {
            QDBusReply<QString> reply = call.reply();
            user->setCurrentAvatar(reply.value());
        }
        watcher->deleteLater();
    });
}

void AccountsWorker::createAccount(const User *user)
{
    qDebug() << "create account";
    Q_EMIT requestFrameAutoHide(false);

    QFutureWatcher<CreationResult*> *watcher = new QFutureWatcher<CreationResult*>(this);
    connect(watcher, &QFutureWatcher<CreationResult*>::finished, [this, watcher] {
        CreationResult *result = watcher->result();
        m_userModel->setAllGroups(m_accountsInter->GetGroups());
        Q_EMIT accountCreationFinished(result);
        Q_EMIT requestFrameAutoHide(true);
        Q_EMIT requestMainWindowEnabled(true);
        watcher->deleteLater();
    });

    QFuture<CreationResult*> future = QtConcurrent::run(this, &AccountsWorker::createAccountInternal, user);
    Q_EMIT requestMainWindowEnabled(false);
    watcher->setFuture(future);
}

void AccountsWorker::setAvatar(User *user, const QString &iconPath)
{
    qDebug() << "set account avatar";
    AccountsUser *ui = m_userInters[user];
    Q_ASSERT(ui);

    ui->SetIconFile(iconPath);
}

void AccountsWorker::setFullname(User *user, const QString &fullname)
{
    AccountsUser *ui = m_userInters[user];
    Q_ASSERT(ui);

    Q_EMIT requestFrameAutoHide(false);

    QDBusPendingCall call = ui->SetFullName(fullname);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (!call.isError()) {
            Q_EMIT accountFullNameChangeFinished();
        }

        Q_EMIT requestFrameAutoHide(true);
        watcher->deleteLater();
    });
}

void AccountsWorker::deleteAccount(User *user, const bool deleteHome)
{
    QDBusPendingReply<> reply = m_accountsInter->DeleteUser(user->name(), deleteHome);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error().message();
    } else {
        getAllGroups();
        Q_EMIT m_userModel->deleteUserSuccess();
    }
}

void AccountsWorker::setAutoLogin(User *user, const bool autoLogin)
{
    AccountsUser *ui = m_userInters[user];
    Q_ASSERT(ui);

    // because this operate need root permission, we must wait for finished and refersh result
    Q_EMIT requestFrameAutoHide(false);

    QDBusPendingCall call = ui->SetAutomaticLogin(autoLogin);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            Q_EMIT user->autoLoginChanged(user->autoLogin());
        }

        Q_EMIT requestFrameAutoHide(true);
        watcher->deleteLater();
    });
}

void AccountsWorker::loadUserList()
{
    onUserListChanged(m_accountsInter->userList());
}

void AccountsWorker::onUserListChanged(const QStringList &userList)
{
    int count = 0;
    static bool first = true;
    for (const QString &path : userList) {
        if (!m_userModel->contains(path)) {
            count++;
            addUser(path);
            if (count > 50 && first) {
                first = false;
                break; // 第一次打开用户列表只加载50个用户信息
            } else if (count > 2 && !first) {
                break; // 滚动条每滑动一次，加载2个用户
            }
        }
    }
}

void AccountsWorker::setPassword(User *user, const QString &oldpwd, const QString &passwd)
{
    QProcess process;
    QProcessEnvironment env;
    env.insert("LC_ALL", "C");
    process.setProcessEnvironment(env);
    process.start("/bin/bash", QStringList() << "-c" << QString("passwd"));
    if (user->passwordStatus() == NO_PASSWORD) {
        process.write(QString("%1\n%2\n").arg(passwd).arg(passwd).toLatin1());
    } else {
        process.write(QString("%1\n%2\n%3").arg(oldpwd).arg(passwd).arg(passwd).toLatin1());
    }
    process.closeWriteChannel();
    process.waitForFinished();

    // process.exitCode() = 10 表示密码修改失败
    int exitCode = process.exitCode();
    if (exitCode == 10) {
        QString errortxt = process.readAllStandardError();
        qDebug() << errortxt;
        if (errortxt.contains("it is WAY too short") || errortxt.contains("You must choose a longer password")) {
            exitCode = 11;
        } else if (errortxt.contains("is too similar to the old one") || errortxt.contains("new and old password are too similar")) {
            exitCode = 12;
        } else if (errortxt.contains("Password unchanged")) {
            exitCode = 13;
        } else if (errortxt.contains("it is too simplistic/systematic") || errortxt.contains("new password is too simple")) {
            exitCode = 14;
        } else if (errortxt.contains("Password has been already used. Choose another")) {
            exitCode = 15;
        } else if (errortxt.contains("it is based on a dictionary word")) {
            exitCode = 16;
        } else if (errortxt.contains("it is based on a (reversed) dictionary word")) {
            exitCode = 17;
        } else if (errortxt.contains("Authentication token manipulation error")) {
            exitCode = 10;
        } else {
            exitCode = 20;
        }
    }

    Q_EMIT user->passwordModifyFinished(exitCode);
}

void AccountsWorker::deleteUserIcon(User *user, const QString &iconPath)
{
    AccountsUser *userInter = m_userInters[user];
    Q_ASSERT(userInter);

    userInter->DeleteIconFile(iconPath);
}

void AccountsWorker::addUser(const QString &userPath)
{
    AccountsUser *userInter = new AccountsUser(AccountsService, userPath, QDBusConnection::systemBus(), this);
    userInter->setSync(false);

    User *user = new User(this);

    connect(userInter, &AccountsUser::UserNameChanged, user, [=](const QString &name) {
        user->setName(name);
        user->setOnline(m_onlineUsers.contains(name));
        user->setIsCurrentUser(name == m_currentUserName);
#ifdef DCC_ENABLE_ADDOMAIN
        checkADUser();
#endif
    });

    connect(userInter, &AccountsUser::AutomaticLoginChanged, user, &User::setAutoLogin);
    connect(userInter, &AccountsUser::IconListChanged, user, &User::setAvatars);
    connect(userInter, &AccountsUser::IconFileChanged, user, &User::setCurrentAvatar);
    connect(userInter, &AccountsUser::FullNameChanged, user, &User::setFullname);
    connect(userInter, &AccountsUser::NoPasswdLoginChanged, user, &User::setNopasswdLogin);
    connect(userInter, &AccountsUser::PasswordStatusChanged, user, &User::setPasswordStatus);
    connect(userInter, &AccountsUser::CreatedTimeChanged, user, &User::setCreatedTime);
    connect(userInter, &AccountsUser::GroupsChanged, user, &User::setGroups);
    connect(userInter, &AccountsUser::AccountTypeChanged, user, &User::setUserType);
    connect(userInter, &AccountsUser::MaxPasswordAgeChanged, user, &User::setPasswordAge);

    user->setName(userInter->userName());
    user->setFullname(userInter->fullName());
    user->setAutoLogin(userInter->automaticLogin());
    user->setAvatars(userInter->iconList());
    user->setGroups(userInter->groups());
    user->setCurrentAvatar(userInter->iconFile());
    user->setNopasswdLogin(userInter->noPasswdLogin());
    user->setPasswordStatus(userInter->passwordStatus());
    user->setCreatedTime(userInter->createdTime());
    user->setUserType(userInter->accountType());
    user->setPasswordAge(userInter->maxPasswordAge());
    user->setIsPasswordExpired(userInter->IsPasswordExpired());

    m_userInters[user] = userInter;
    m_userModel->addUser(userPath, user);
}

void AccountsWorker::removeUser(const QString &userPath)
{
    for (AccountsUser *userInter : m_userInters.values()) {
        if (userInter->path() == userPath) {
            User *user = m_userInters.key(userInter);
            user->deleteLater();

            m_userInters.remove(user);
            m_userModel->removeUser(userPath);

            return;
        }
    }
}

void AccountsWorker::setNopasswdLogin(User *user, const bool nopasswdLogin)
{
    AccountsUser *userInter = m_userInters[user];
    Q_ASSERT(userInter);

    Q_EMIT requestFrameAutoHide(false);

    QDBusPendingCall call = userInter->EnableNoPasswdLogin(nopasswdLogin);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            Q_EMIT user->nopasswdLoginChanged(user->nopasswdLogin());
        }

        Q_EMIT requestFrameAutoHide(true);
        watcher->deleteLater();
    });
}

void AccountsWorker::setMaxPasswordAge(User *user, const int maxAge)
{
    AccountsUser *userInter = m_userInters[user];
    Q_ASSERT(userInter);

    QDBusPendingCall call = userInter->SetMaxPasswordAge(maxAge);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            Q_EMIT user->passwordAgeChanged(user->passwordAge());
        }
        watcher->deleteLater();
    });
}

#ifdef DCC_ENABLE_ADDOMAIN
void AccountsWorker::refreshADDomain()
{
    QProcess *process = new QProcess(this);
    process->start("/opt/pbis/bin/enum-users");

    connect(process, &QProcess::readyReadStandardOutput, this, [=] {
        QRegularExpression re("Name:\\s+(\\w+)");
        QRegularExpressionMatch match = re.match(process->readAll());
        m_userModel->setIsJoinADDomain(match.hasMatch());
    });

    connect(process, static_cast<void (QProcess::*)(int)>(&QProcess::finished), process, &QProcess::deleteLater);
}
#endif

#ifdef DCC_ENABLE_ADDOMAIN
void AccountsWorker::ADDomainHandle(const QString &server, const QString &admin, const QString &password)
{
    const bool isJoin = m_userModel->isJoinADDomain();
    int exitCode = 0;
    if (isJoin) {
        exitCode = QProcess::execute("pkexec", QStringList() << "/opt/pbis/bin/domainjoin-cli" << "leave" << "--disable" << "ssh");
    } else {
        // for safety, restart lwsmd service before join AD Domain
        QProcess::execute("pkexec", QStringList() << "/bin/systemctl" << "restart" << "lwsmd");
        exitCode = QProcess::execute("pkexec", QStringList() << "/opt/pbis/bin/domainjoin-cli" << "join" << "--disable" << "ssh" << server << admin << password);
    }

    QString message;

    if (!exitCode) {
        message = isJoin ? tr("Your host was removed from the domain server successfully")
                         : tr("Your host joins the domain server successfully");

        // Additional operation, need to initialize the user's settings
        if (!isJoin) {
            QProcess::execute("pkexec", QStringList() << "/opt/pbis/bin/config" << "UserDomainPrefix" << "ADS");
            QProcess::execute("pkexec", QStringList() << "/opt/pbis/bin/config" << "LoginShellTemplate" << "/bin/bash");
        }
        // save config
        QFile file("/etc/deepin/dde-session-ui.conf");
        QFile tmpFile("/tmp/.dde-session-ui.conf");

        if (file.exists() && file.open(QIODevice::Text | QIODevice::ReadOnly)) {
            qDebug() << file.copy("/tmp/.dde-session-ui.conf");
        }
        if (tmpFile.open(QIODevice::Text | QIODevice::ReadWrite)) {
            QSettings setting("/tmp/.dde-session-ui.conf", QSettings::IniFormat);
            setting.setValue("loginPromptInput", !isJoin);
            setting.sync();
            QProcess::execute("pkexec", QStringList() << "cp" << "/tmp/.dde-session-ui.conf" << "/etc/deepin/dde-session-ui.conf");
            tmpFile.remove();
        }
    } else {
        message = isJoin ? tr("Your host failed to leave the domain server")
                         : tr("Your host failed to join the domain server");
    }

    m_notifyInter->Notify("", QDateTime::currentMSecsSinceEpoch(), exitCode ? "dialog-warning" : "dialog-ok", tr("AD domain settings"), message, QStringList(), QVariantMap(), 0);
    refreshADDomain();
}
#endif

void AccountsWorker::updateUserOnlineStatus(const QList<QDBusObjectPath> &paths)
{
    m_onlineUsers.clear();

    for (const QDBusObjectPath &path : paths) {
        Session tmpSession(DisplayManagerService, path.path(), QDBusConnection::systemBus());
        m_onlineUsers << tmpSession.userName();
    }

    for (User *user : m_userModel->userList()) {
        user->setOnline(m_onlineUsers.contains(user->name()));
    }

#ifdef DCC_ENABLE_ADDOMAIN
    checkADUser();
#endif
}

#ifdef DCC_ENABLE_ADDOMAIN
void AccountsWorker::checkADUser()
{
    // AD User is not in native user list, but session list have it.
    bool isADUser = false;

    QStringList userList;

    for (User *user : m_userModel->userList()) {
        userList << user->name();
    }

    for (const QString &u : m_onlineUsers) {
        if (!userList.contains(u)) {
            isADUser = true;
            break;
        }
    }

    m_userModel->setADUserLogind(isADUser);
}
#endif

CreationResult *AccountsWorker::createAccountInternal(const User *user)
{
    CreationResult *result = new CreationResult;

    // validate username
    QDBusPendingReply<bool, QString, int> reply = m_accountsInter->IsUsernameValid(user->name());
    reply.waitForFinished();
    if (reply.isError()) {
        result->setType(CreationResult::UserNameError);
        result->setMessage(reply.error().message());

        return result;
    }
    bool validation = reply.argumentAt(0).toBool();
    if (!validation) {
        result->setType(CreationResult::UserNameError);
        result->setMessage(dgettext("dde-daemon", reply.argumentAt(1).toString().toUtf8().data()));
        return result;
    }

    // validate password
    if (user->password() != user->repeatPassword()) {
        result->setType(CreationResult::PasswordMatchError);
        result->setMessage(tr("Password not match"));
        return result;
    }

    // default FullName is empty string
    QDBusObjectPath path = m_accountsInter->CreateUser(user->name(), user->fullname(), user->userType());

    const QString userPath = path.path();
    if (userPath.isEmpty() || userPath.isNull()) {
        result->setType(CreationResult::UnknownError);
        result->setMessage("no method call result on CreateUser");
        return result;
    }

    AccountsUser *userDBus = new AccountsUser("com.deepin.daemon.Accounts", userPath, QDBusConnection::systemBus(), this);
    if (!userDBus->isValid()) {
        result->setType(CreationResult::UnknownError);
        result->setMessage("user dbus is still not valid.");

        return result;
    }

    //TODO(hualet): better to check all the call results.
    bool sifResult = !userDBus->SetIconFile(user->currentAvatar()).isError();
    bool spResult = !userDBus->SetPassword(cryptUserPassword(user->password())).isError();
    bool groupResult = true;
    if (IsServerSystem) {
        groupResult = !userDBus->SetGroups(user->groups()).isError();
    }

    if (!sifResult || !spResult || !groupResult) {
        result->setType(CreationResult::UnknownError);
        if (!sifResult) result->setMessage("set icon file for new created user failed.");
        if (!spResult) result->setMessage("set password for new created user failed");
        if (!groupResult) result->setMessage("set group for new created user failed");
        return result;
    }

    return result;
}

QString AccountsWorker::cryptUserPassword(const QString &password)
{
    /*
        NOTE(kirigaya): Password is a combination of salt and crypt function.
        slat is begin with $6$, 16 byte of random values, at the end of $.
        crypt function will return encrypted values.
     */

    const QString seedchars("./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    char salt[] = "$6$................$";

    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(0, seedchars.size() - 1);//seedchars.size()是64，生成随机数的范围应该写成[0, 63]。

    // Random access to a character in a restricted list
    for (int i = 0; i != 16; i++) {
        salt[3 + i] = seedchars.at(uniform_dist(e1)).toLatin1();
    }

    return crypt(password.toUtf8().data(), salt);
}
