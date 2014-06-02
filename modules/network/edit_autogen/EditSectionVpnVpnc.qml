// This file is automatically generated, please don't edit manully.
import QtQuick 2.1
import Deepin.Widgets 1.0
import "../edit"

BaseEditSection { 
    id: sectionVpnVpnc
    virtualSection: "vs-vpn-vpnc"
    
    header.sourceComponent: EditDownArrowHeader{
        text: dsTr("VPN")
    }

    content.sourceComponent: Column { 
        EditLineTextInput {
            id: lineAliasVpnVpncIpsecGateway
            connectionSession: sectionVpnVpnc.connectionSession
            data: sectionVpnVpnc.data
            availableSections: sectionVpnVpnc.availableSections
            availableKeys: sectionVpnVpnc.availableKeys
            errors: sectionVpnVpnc.errors
            section: "alias-vpn-vpnc"
            key: "IPSec gateway"
            text: dsTr("Gateway")
        }
        EditLineTextInput {
            id: lineAliasVpnVpncXauthUsername
            connectionSession: sectionVpnVpnc.connectionSession
            data: sectionVpnVpnc.data
            availableSections: sectionVpnVpnc.availableSections
            availableKeys: sectionVpnVpnc.availableKeys
            errors: sectionVpnVpnc.errors
            section: "alias-vpn-vpnc"
            key: "Xauth username"
            text: dsTr("Username")
        }
        EditLineComboBox {
            id: lineAliasVpnVpncXauthPasswordFlags
            connectionSession: sectionVpnVpnc.connectionSession
            data: sectionVpnVpnc.data
            availableSections: sectionVpnVpnc.availableSections
            availableKeys: sectionVpnVpnc.availableKeys
            errors: sectionVpnVpnc.errors
            section: "alias-vpn-vpnc"
            key: "Xauth password-flags"
            text: dsTr("Ask for Pwd")
        }
        EditLinePasswordInput {
            id: lineAliasVpnVpncXauthPassword
            connectionSession: sectionVpnVpnc.connectionSession
            data: sectionVpnVpnc.data
            availableSections: sectionVpnVpnc.availableSections
            availableKeys: sectionVpnVpnc.availableKeys
            errors: sectionVpnVpnc.errors
            section: "alias-vpn-vpnc"
            key: "Xauth password"
            text: dsTr("User Pwd")
        }
        EditLineTextInput {
            id: lineAliasVpnVpncIpsecId
            connectionSession: sectionVpnVpnc.connectionSession
            data: sectionVpnVpnc.data
            availableSections: sectionVpnVpnc.availableSections
            availableKeys: sectionVpnVpnc.availableKeys
            errors: sectionVpnVpnc.errors
            section: "alias-vpn-vpnc"
            key: "IPSec ID"
            text: dsTr("Group Name")
        }
        EditLineComboBox {
            id: lineAliasVpnVpncIpsecSecretFlags
            connectionSession: sectionVpnVpnc.connectionSession
            data: sectionVpnVpnc.data
            availableSections: sectionVpnVpnc.availableSections
            availableKeys: sectionVpnVpnc.availableKeys
            errors: sectionVpnVpnc.errors
            section: "alias-vpn-vpnc"
            key: "IPSec secret-flags"
            text: dsTr("Ask for Pwd")
        }
        EditLinePasswordInput {
            id: lineAliasVpnVpncIpsecSecret
            connectionSession: sectionVpnVpnc.connectionSession
            data: sectionVpnVpnc.data
            availableSections: sectionVpnVpnc.availableSections
            availableKeys: sectionVpnVpnc.availableKeys
            errors: sectionVpnVpnc.errors
            section: "alias-vpn-vpnc"
            key: "IPSec secret"
            text: dsTr("Group Pwd")
        }
        EditLineSwitchButton {
            id: lineAliasVpnVpncVkHybridAuthmode
            connectionSession: sectionVpnVpnc.connectionSession
            data: sectionVpnVpnc.data
            availableSections: sectionVpnVpnc.availableSections
            availableKeys: sectionVpnVpnc.availableKeys
            errors: sectionVpnVpnc.errors
            section: "alias-vpn-vpnc"
            key: "vk-hybrid-authmode"
            text: dsTr("Use Hybrid Authentication")
        }
        EditLineFileChooser {
            id: lineAliasVpnVpncCaFile
            connectionSession: sectionVpnVpnc.connectionSession
            data: sectionVpnVpnc.data
            availableSections: sectionVpnVpnc.availableSections
            availableKeys: sectionVpnVpnc.availableKeys
            errors: sectionVpnVpnc.errors
            section: "alias-vpn-vpnc"
            key: "CA-File"
            text: dsTr("CA File")
        }
    }
}
