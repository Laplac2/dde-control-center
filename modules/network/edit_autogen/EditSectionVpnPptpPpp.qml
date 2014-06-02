// This file is automatically generated, please don't edit manully.
import QtQuick 2.1
import Deepin.Widgets 1.0
import "../edit"

BaseEditSection { 
    id: sectionVpnPptpPpp
    virtualSection: "vs-vpn-pptp-ppp"
    
    header.sourceComponent: EditDownArrowHeader{
        text: dsTr("VPN PPP")
    }

    content.sourceComponent: Column { 
        EditLineSwitchButton {
            id: lineAliasVpnPptpPppVkRequireMppe
            connectionSession: sectionVpnPptpPpp.connectionSession
            data: sectionVpnPptpPpp.data
            availableSections: sectionVpnPptpPpp.availableSections
            availableKeys: sectionVpnPptpPpp.availableKeys
            errors: sectionVpnPptpPpp.errors
            section: "alias-vpn-pptp-ppp"
            key: "vk-require-mppe"
            text: dsTr("Use MPPE")
        }
        EditLineComboBox {
            id: lineAliasVpnPptpPppVkMppeSecurity
            connectionSession: sectionVpnPptpPpp.connectionSession
            data: sectionVpnPptpPpp.data
            availableSections: sectionVpnPptpPpp.availableSections
            availableKeys: sectionVpnPptpPpp.availableKeys
            errors: sectionVpnPptpPpp.errors
            section: "alias-vpn-pptp-ppp"
            key: "vk-mppe-security"
            text: dsTr("Security")
        }
        EditLineSwitchButton {
            id: lineAliasVpnPptpPppMppeStateful
            connectionSession: sectionVpnPptpPpp.connectionSession
            data: sectionVpnPptpPpp.data
            availableSections: sectionVpnPptpPpp.availableSections
            availableKeys: sectionVpnPptpPpp.availableKeys
            errors: sectionVpnPptpPpp.errors
            section: "alias-vpn-pptp-ppp"
            key: "mppe-stateful"
            text: dsTr("Stateful MPPE")
        }
        EditLineSwitchButton {
            id: lineAliasVpnPptpPppRefuseEap
            connectionSession: sectionVpnPptpPpp.connectionSession
            data: sectionVpnPptpPpp.data
            availableSections: sectionVpnPptpPpp.availableSections
            availableKeys: sectionVpnPptpPpp.availableKeys
            errors: sectionVpnPptpPpp.errors
            section: "alias-vpn-pptp-ppp"
            key: "refuse-eap"
            text: dsTr("Refuse EAP Authentication")
            alwaysUpdate: true
        }
        EditLineSwitchButton {
            id: lineAliasVpnPptpPppRefusePap
            connectionSession: sectionVpnPptpPpp.connectionSession
            data: sectionVpnPptpPpp.data
            availableSections: sectionVpnPptpPpp.availableSections
            availableKeys: sectionVpnPptpPpp.availableKeys
            errors: sectionVpnPptpPpp.errors
            section: "alias-vpn-pptp-ppp"
            key: "refuse-pap"
            text: dsTr("Refuse PAP Authentication")
            alwaysUpdate: true
        }
        EditLineSwitchButton {
            id: lineAliasVpnPptpPppRefuseChap
            connectionSession: sectionVpnPptpPpp.connectionSession
            data: sectionVpnPptpPpp.data
            availableSections: sectionVpnPptpPpp.availableSections
            availableKeys: sectionVpnPptpPpp.availableKeys
            errors: sectionVpnPptpPpp.errors
            section: "alias-vpn-pptp-ppp"
            key: "refuse-chap"
            text: dsTr("Refuse CHAP Authentication")
            alwaysUpdate: true
        }
        EditLineSwitchButton {
            id: lineAliasVpnPptpPppRefuseMschap
            connectionSession: sectionVpnPptpPpp.connectionSession
            data: sectionVpnPptpPpp.data
            availableSections: sectionVpnPptpPpp.availableSections
            availableKeys: sectionVpnPptpPpp.availableKeys
            errors: sectionVpnPptpPpp.errors
            section: "alias-vpn-pptp-ppp"
            key: "refuse-mschap"
            text: dsTr("Refuse MSCHAP Authentication")
            alwaysUpdate: true
        }
        EditLineSwitchButton {
            id: lineAliasVpnPptpPppRefuseMschapv2
            connectionSession: sectionVpnPptpPpp.connectionSession
            data: sectionVpnPptpPpp.data
            availableSections: sectionVpnPptpPpp.availableSections
            availableKeys: sectionVpnPptpPpp.availableKeys
            errors: sectionVpnPptpPpp.errors
            section: "alias-vpn-pptp-ppp"
            key: "refuse-mschapv2"
            text: dsTr("Refuse MSCHAPv2 Authentication")
        }
        EditLineSwitchButton {
            id: lineAliasVpnPptpPppNobsdcomp
            connectionSession: sectionVpnPptpPpp.connectionSession
            data: sectionVpnPptpPpp.data
            availableSections: sectionVpnPptpPpp.availableSections
            availableKeys: sectionVpnPptpPpp.availableKeys
            errors: sectionVpnPptpPpp.errors
            section: "alias-vpn-pptp-ppp"
            key: "nobsdcomp"
            text: dsTr("No BSD Data Compression")
        }
        EditLineSwitchButton {
            id: lineAliasVpnPptpPppNodeflate
            connectionSession: sectionVpnPptpPpp.connectionSession
            data: sectionVpnPptpPpp.data
            availableSections: sectionVpnPptpPpp.availableSections
            availableKeys: sectionVpnPptpPpp.availableKeys
            errors: sectionVpnPptpPpp.errors
            section: "alias-vpn-pptp-ppp"
            key: "nodeflate"
            text: dsTr("No Deflate Data Compression")
        }
        EditLineSwitchButton {
            id: lineAliasVpnPptpPppNoVjComp
            connectionSession: sectionVpnPptpPpp.connectionSession
            data: sectionVpnPptpPpp.data
            availableSections: sectionVpnPptpPpp.availableSections
            availableKeys: sectionVpnPptpPpp.availableKeys
            errors: sectionVpnPptpPpp.errors
            section: "alias-vpn-pptp-ppp"
            key: "no-vj-comp"
            text: dsTr("No TCP Header Compression")
        }
        EditLineSwitchButton {
            id: lineAliasVpnPptpPppVkEnableLcpEcho
            connectionSession: sectionVpnPptpPpp.connectionSession
            data: sectionVpnPptpPpp.data
            availableSections: sectionVpnPptpPpp.availableSections
            availableKeys: sectionVpnPptpPpp.availableKeys
            errors: sectionVpnPptpPpp.errors
            section: "alias-vpn-pptp-ppp"
            key: "vk-enable-lcp-echo"
            text: dsTr("Send PPP Echo Packets")
        }
    }
}
