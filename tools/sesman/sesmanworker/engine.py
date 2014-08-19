#!/usr/bin/python
# -*- coding: UTF-8 -*-

from sesmanconf import TR

class AuthenticationFailed(Exception): pass

from model import *
from logger import Logger

from password import PASSWORD1, PASSWORD2, PASSWORD3, PASSWORD4, PASSWORD5, PASSWORD6, PASSWORD7, PASSWORD8, PASSWORD9

class ChallengeInfo(object):
    def __init__(self, message=None, promptEcho=None):
        self.message = message
        self.promptEcho = promptEcho

class Engine(object):
    config_app_params = {
        'notepaduser@NOTEPAD':
        {
            'NOTEPAD_administrateur@qa@10.10.146.78:APP':
                AppParamsInfo(
                    workingdir = u'c:\\windows',
                    program = u'c:\\windows\\system32\\notepad.exe',
                    params = None,
                    authmechanism = None
                ),
            'NOTEPAD_admin@10.10.46.78:APP':
                AppParamsInfo(
                    workingdir = u'c:\\windows',
                    program = u'c:\\windows\\system32\\notepad.exe',
                    params = None,
                    authmechanism = None
                ),
            'NOTEPAD_qa\\administrateur@10.10.46.78:APP':
                AppParamsInfo(
                    workingdir = u'c:\\windows',
                    program = u'c:\\windows\\system32\\notepad.exe',
                    params = None,
                    authmechanism = None
                )
        }
    }

    config_effective_targets = {
        'NOTEPAD_administrateur@qa@10.10.146.78:APP':
            RightInfo(
                account = AccountInfo(
                    isAgentForwardable = u'False',
                    login = u'administrateur@qa',
                    password = PASSWORD1,
                    pubkey = None,
                    isKeyAuth = None
                ),
                group_targets = [],
                target_groups = u'',
                resource = ResourceInfo(
                    device = DeviceInfo(
                        cn = u'10.10.146.78',
                        uid = u'140ee23607907e970800279eed97',
                        deviceAlias = u'',
                        host = u'10.10.146.78',
                        isKeyAuth = None
                    ),
                    application = None,
                    service = ServiceInfo(
                        authmechanism = None,
                        protocol = ProtocolInfo(
                            cn = u'RDP'
                        ),
                        cn = u'RDP',
                        port = u'3389'
                    )
                ),
                auth_mode = None,
                authorization = None,
                service_login = None,
                subprotocols = [],
                deconnection_time = None
            ),
        'NOTEPAD_admin@10.10.46.78:APP':
            RightInfo(
                account = AccountInfo(
                    isAgentForwardable = u'False',
                    login = u'admin',
                    password = PASSWORD2,
                    pubkey = None,
                    isKeyAuth = None
                ),
                group_targets = [],
                target_groups = u'',
                resource = ResourceInfo(
                    device = DeviceInfo(
                        cn = u'10.10.46.78',
                        uid = u'140ee23607907e970800279eed97',
                        deviceAlias = u'',
                        host = u'10.10.46.78',
                        isKeyAuth = None
                    ),
                    application = None,
                    service = ServiceInfo(
                        authmechanism = None,
                        protocol = ProtocolInfo(
                            cn = u'RDP'
                        ),
                        cn = u'RDP',
                        port = u'3389'
                    )
                ),
                auth_mode = None,
                authorization = None,
                service_login = None,
                subprotocols = [],
                deconnection_time = None
            ),
        'NOTEPAD_qa\\administrateur@10.10.46.78:APP':
            RightInfo(
                account = AccountInfo(
                    isAgentForwardable = u'False',
                    login = u'qa\\administrateur',
                    password = PASSWORD1,
                    pubkey = None,
                    isKeyAuth = None
                ),
                group_targets = [],
                target_groups = u'',
                resource = ResourceInfo(
                    device = DeviceInfo(
                        cn = u'10.10.46.78',
                        uid = u'140ee23607907e970800279eed97',
                        deviceAlias = u'',
                        host = u'10.10.46.78',
                        isKeyAuth = None
                    ),
                    application = None,
                    service = ServiceInfo(
                        authmechanism = None,
                        protocol = ProtocolInfo(
                            cn = u'RDP'
                        ),
                        cn = u'RDP',
                        port = u'3389'
                    )
                ),
                auth_mode = None,
                authorization = None,
                service_login = None,
                subprotocols = [],
                deconnection_time = None
            )
    }   # config_effective_targets

    config_rights = {
        'itnl_internal@bouncer2:RDP': RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'internal',
                password = u'internal',
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'bouncer'
                )
            ],
            target_groups = u'bouncer',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'bouncer2',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'bouncer2',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = False,
                isRecorded = False
            ),
            service_login = u'internal@bouncer2:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'itnl_internal@test_card:RDP': RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'internal',
                password = u'internal',
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'card'
                )
            ],
            target_groups = u'card',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'test_card',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'test_card',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = False,
                isRecorded = False
            ),
            service_login = u'internal@test_card:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'itnl_internal@widget2_message:RDP': RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'internal',
                password = u'internal',
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'message'
                )
            ],
            target_groups = u'message',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'widget2_message',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'widget2_message',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = False,
                isRecorded = False
            ),
            service_login = u'internal@widget2_message:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'itnl_replay@autotest:RDP': RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'replay',
                password = u'password',
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'replay'
                )
            ],
            target_groups = u'replay',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'autotest',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'autotest',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = False,
                isRecorded = False
            ),
            service_login = u'replay@autotest:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'itnl_internal@widgettest:RDP': RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'internal',
                password = u'password',
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'replay'
                )
            ],
            target_groups = u'internal',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'widgettest',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'widgettest',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = False,
                isRecorded = False
            ),
            service_login = u'internal@widgettest:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'notepaduser@NOTEPAD:APP': RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'notepaduser',
                password = u'notepaduserpass',
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win2'
                )
            ],
            target_groups = u'win2',
            resource = ResourceInfo(
                device = None,
                application = ApplicationInfo(
                    cn=u'NOTEPAD',
                    uid=u'1412c8ff638694ee0800279eed97'
                ),
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = False,
                isRecorded = False
            ),
            service_login = u'notepaduser@NOTEPAD',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'rec_w2k_administrateur@10.10.46.64:RDP' : RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'administrateur',
                password = PASSWORD3,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win1'
                )
            ],
            target_groups = u'win1',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.46.64',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.46.64',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = True,
                isRecorded = True
            ),
            service_login = u'administrateur@10.10.46.64:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'rec_w2k_administrateur@10.10.47.39:RDP' : RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'administrateur',
                password = PASSWORD2,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win1'
                )
            ],
            target_groups = u'win1',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.47.39',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.47.39',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = True,
                isRecorded = True
            ),
            service_login = u'administrateur@10.10.47.39:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'rec_w2k3_administrateur@10.10.47.205:RDP' : RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'administrateur',
                password = PASSWORD2,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win1'
                )
            ],
            target_groups = u'win1',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.47.205',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.47.205',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = True,
                isRecorded = True
            ),
            service_login = u'administrateur@10.10.47.205:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'rec_w2k8_administrateur@10.10.47.89:RDP' : RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'administrateur',
                password = PASSWORD2,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win1'
                )
            ],
            target_groups = u'win1',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.47.89',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.47.89',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = True,
                isRecorded = True
            ),
            service_login = u'administrateur@10.10.47.89:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'rec_w2k3_any@10.10.46.70:VNC': RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'any',
                password = PASSWORD4,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'vnc1'
                )
            ],
            target_groups = u'vnc1',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.46.70',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.46.70',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'VNC'
                    ),
                    cn = u'VNC',
                    port = u'5900'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = True,
                isRecorded = True
            ),
            service_login = u'any@10.10.46.70:VNC',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'VNC',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'rec_w2k3_qa\\administrateur@10.10.46.70:RDP' : RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'qa\\administrateur',
                password = PASSWORD1,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win1'
                ),
                GroupTargetInfo(
                    cn = u'win2'
                )
            ],
            target_groups = u'win1;win2;',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.46.70',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.46.70',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = True,
                isRecorded = True
            ),
            service_login = u'qa\\administrateur@10.10.46.70:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'rec_w2k8_qa\\administrateur@10.10.46.78:RDP' : RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'qa\\administrateur',
                password = PASSWORD1,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win1'
                ),
                GroupTargetInfo(
                    cn = u'win2'
                )
            ],
            target_groups = u'win1;win2;',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.46.78',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.46.78',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = True,
                isRecorded = True
            ),
            service_login = u'qa\\administrateur@10.10.46.78:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'rec_wxp_administrateur@10.10.47.175:RDP' : RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'administrateur',
                password = PASSWORD2,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win1'
                )
            ],
            target_groups = u'win1',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.47.175',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.47.175',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = True,
                isRecorded = True
            ),
            service_login = u'administrateur@10.10.47.175:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'rec_w2k12_administrateur@10.10.47.132:RDP' : RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'administrateur',
                password = PASSWORD3,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win2012'
                )
            ],
            target_groups = u'win2012',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.47.132',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.47.132',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = True,
                isRecorded = True
            ),
            service_login = u'administrateur@10.10.47.132:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'w2k_administrateur@10.10.46.64:RDP' : RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'administrateur',
                password = PASSWORD3,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win1'
                )
            ],
            target_groups = u'win1',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.46.64',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.46.64',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = False,
                isRecorded = False
            ),
            service_login = u'administrateur@10.10.46.64:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'w2k_administrateur@10.10.47.39:RDP' : RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'administrateur',
                password = PASSWORD2,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win1'
                )
            ],
            target_groups = u'win1',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.47.39',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.47.39',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = False,
                isRecorded = False
            ),
            service_login = u'administrateur@10.10.47.39:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'w2k3_administrateur@10.10.47.205:RDP' : RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'administrateur',
                password = PASSWORD2,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win1'
                )
            ],
            target_groups = u'win1',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.47.205',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.47.205',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = False,
                isRecorded = False
            ),
            service_login = u'administrateur@10.10.47.205:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'w2k3_any@10.10.42.13:VNC': RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'any',
                password = u'linux',
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'vnc1'
                )
            ],
            target_groups = u'vnc1',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.42.13',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.42.13',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'VNC'
                    ),
                    cn = u'VNC',
                    port = u'5900'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = False,
                isRecorded = False
            ),
            service_login = u'any@10.10.42.13:VNC',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'VNC',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'w2k3_any@10.10.46.70:VNC': RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'any',
                password = PASSWORD4,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'vnc1'
                )
            ],
            target_groups = u'vnc1',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.46.70',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.46.70',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'VNC'
                    ),
                    cn = u'VNC',
                    port = u'5900'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = False,
                isRecorded = False
            ),
            service_login = u'any@10.10.46.70:VNC',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'VNC',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'w2k8_administrateur@qa@10.10.146.78:RDP': RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'administrateur@qa',
                password = PASSWORD1,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win1'
                )
            ],
            target_groups = u'win1',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.146.78',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.146.78',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = False,
                isRecorded = False
            ),
            service_login = u'administrateur@qa@10.10.146.78:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'w2k8_admin@10.10.46.78:RDP' : RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'admin',
                password = u'BadPassword',
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win1'
                ),
                GroupTargetInfo(
                    cn = u'win2'
                )
            ],
            target_groups = u'win1;win2',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.46.78',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.46.78',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = False,
                isRecorded = False
            ),
            service_login = u'admin@10.10.46.78:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'w2k3_qa\\administrateur@10.10.46.70:RDP' : RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'qa\\administrateur',
                password = PASSWORD1,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win1'
                ),
                GroupTargetInfo(
                    cn = u'win2'
                )
            ],
            target_groups = u'win1;win2',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.46.70',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.46.70',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = False,
                isRecorded = False
            ),
            service_login = u'qa\\administrateur@10.10.46.70:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'w2k8_administrateur@10.10.47.89:RDP' : RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'administrateur',
                password = PASSWORD1,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win1'
                ),
                GroupTargetInfo(
                    cn = u'win2'
                )
            ],
            target_groups = u'win1;win2',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.47.89',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.47.89',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = False,
                isRecorded = False
            ),
            service_login = u'administrateur@10.10.47.89:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'w2k8_qa\\administrateur@10.10.46.78:RDP' : RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'qa\\administrateur',
                password = PASSWORD1,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win1'
                ),
                GroupTargetInfo(
                    cn = u'win2'
                )
            ],
            target_groups = u'win1;win2',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.46.78',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.46.78',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = False,
                isRecorded = False
            ),
            service_login = u'qa\\administrateur@10.10.46.78:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'w2k8_qa\\administrateur@10.10.46.88:RDP' : RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'qa\\administrateur',
                password = PASSWORD1,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win1'
                ),
                GroupTargetInfo(
                    cn = u'win2'
                )
            ],
            target_groups = u'win1;win2',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.46.88',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.46.88',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = False,
                isRecorded = False
            ),
            service_login = u'qa\\administrateur@10.10.46.88:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'wxp_administrateur@10.10.47.175:RDP' : RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'administrateur',
                password = PASSWORD2,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win1'
                )
            ],
            target_groups = u'win1',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.47.175',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.47.175',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = False,
                isRecorded = False
            ),
            service_login = u'administrateur@10.10.47.175:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'w2k8erpm\\administrateur@10.10.46.110:RDP' : RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'QA\\administrateur',
                password = PASSWORD1,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win1'
                ),
                GroupTargetInfo(
                    cn = u'win2'
                )
            ],
            target_groups = u'win1;win2',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.46.110',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.46.110',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = False,
                isRecorded = False
            ),
            service_login = u'QA\\administrateur@10.10.46.110:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        ),
        'w2k8erpm\\special@10.10.46.110:RDP' : RightInfo(
            account = AccountInfo(
                isAgentForwardable = u'0',
                login = u'QA\\Trâçàbïlîtôùñÿê',
                password = PASSWORD9,
                pubkey = None,
                isKeyAuth = None
            ),
            group_targets =
            [
                GroupTargetInfo(
                    cn = u'win1'
                ),
                GroupTargetInfo(
                    cn = u'win2'
                )
            ],
            target_groups = u'win1;win2',
            resource = ResourceInfo(
                device = DeviceInfo(
                    cn = u'10.10.46.110',
                    uid = u'140ee23607907e970800279eed97',
                    deviceAlias = u'',
                    host = u'10.10.46.110',
                    isKeyAuth = None
                ),
                application = None,
                service = ServiceInfo(
                    authmechanism = BlobInfo(
                        data = u''
                    ),
                    protocol = ProtocolInfo(
                        cn = u'RDP'
                    ),
                    cn = u'RDP',
                    port = u'3389'
                )
            ),
            auth_mode = u'NAM',
            authorization = AuthorizationInfo(
                isCritical = False,
                isRecorded = False
            ),
            service_login = u'QA\\Trâçàbïlîtôùñÿê@10.10.46.110:RDP',
            subprotocols =
            [
                SubprotocolInfo(
                    cn = u'RDP',
                    uid = u'140ed5f39235d74d0800279eed97'
                )
            ],
            deconnection_time = u'2099-12-30 23:59:59'
        )
    }   # config_rights

    config_service_logins = {
        'notepaduser@NOTEPAD':
        {
            'rights':
            [
                'NOTEPAD_administrateur@qa@10.10.146.78:APP',
                'NOTEPAD_admin@10.10.46.78:APP',
                'NOTEPAD_qa\\administrateur@10.10.46.78:APP'
            ]
        }
    }

    config_target_password = [
        TargetPasswordInfo(
            account = u'administrateur',
            resource = u'10.10.46.64',
            protocol = u'RDP',
            password = PASSWORD3
        ),
        TargetPasswordInfo(
            account = u'administrateur',
            resource = u'10.10.47.39',
            protocol = u'RDP',
            password = PASSWORD2
        ),
        TargetPasswordInfo(
            account = u'administrateur',
            resource = u'10.10.47.175',
            protocol = u'RDP',
            password = PASSWORD2
        ),
        TargetPasswordInfo(
            account = u'administrateur',
            resource = u'10.10.47.205',
            protocol = u'RDP',
            password = PASSWORD2
        ),
        TargetPasswordInfo(
            account = u'administrateur',
            resource = u'10.10.47.89',
            protocol = u'RDP',
            password = PASSWORD2
        ),
        TargetPasswordInfo(
            account = u'admin',
            resource = u'10.10.46.78',
            protocol = u'RDP',
            password = u'BadPassword'
        ),
        TargetPasswordInfo(
            account = u'administrateur@qa',
            resource = u'10.10.146.78',
            protocol = u'RDP',
            password = PASSWORD1
        ),
        TargetPasswordInfo(
            account = u'qa\\administrateur',
            resource = u'10.10.46.70',
            protocol = u'RDP',
            password = PASSWORD1
        ),
        TargetPasswordInfo(
            account = u'any',
            resource = u'10.10.42.13',
            protocol = u'VNC',
            password = u'linux'
        ),
        TargetPasswordInfo(
            account = u'any',
            resource = u'10.10.46.70',
            protocol = u'VNC',
            password = PASSWORD4
        ),
        TargetPasswordInfo(
            account = u'administrateur',
            resource = u'10.10.47.89',
            protocol = u'RDP',
            password = PASSWORD1
        ),
        TargetPasswordInfo(
            account = u'internal',
            resource = u'bouncer2',
            protocol = u'RDP',
            password = u'internal'
        ),
        TargetPasswordInfo(
            account = u'internal',
            resource = u'test_card',
            protocol = u'RDP',
            password = u'internal'
        ),
        TargetPasswordInfo(
            account = u'internal',
            resource = u'widget2_message',
            protocol = u'RDP',
            password = u'internal'
        ),
        TargetPasswordInfo(
            account = u'replay',
            resource = u'autotest',
            protocol = u'RDP',
            password = u'password'
        ),
        TargetPasswordInfo(
            account = u'internal',
            resource = u'widgettest',
            protocol = u'RDP',
            password = u'password'
        ),
        TargetPasswordInfo(
            account = u'qa\\administrateur',
            resource = u'10.10.46.78',
            protocol = u'RDP',
            password = PASSWORD1
        ),
        TargetPasswordInfo(
            account = u'qa\\administrateur',
            resource = u'10.10.46.88',
            protocol = u'RDP',
            password = PASSWORD1
        ),
        TargetPasswordInfo(
            account = u'QA\\administrateur',
            resource = u'10.10.46.110',
            protocol = u'RDP',
            password = PASSWORD1
        ),
        TargetPasswordInfo(
            account = u'QA\\Trâçàbïlîtôùñÿê',
            # account = u'QA\\noël_et_cébè_et_fran',
            resource = u'10.10.46.110',
            protocol = u'RDP',
            password = PASSWORD9
        ),
    ]

    config_users = {
        'a':
        {
            'is_x509_connected': False,
            'x509_authenticate': False,
            'passthrough_authenticate': False,
            'password': 'apass',
            'preferredLanguage': u'en',
            'rights':
            [
                'notepaduser@NOTEPAD:APP'
            ]
        },
        'bad':
        {
            'is_x509_connected': False,
            'x509_authenticate': False,
            'passthrough_authenticate': False,
            'password': 'bad',
            'preferredLanguage': u'en',
            'rights':
            [
                'w2k8_administrateur@qa@10.10.146.78:RDP',
                'w2k8_admin@10.10.46.78:RDP',
            ]
        },
        'internal':
        {
            'is_x509_connected': False,
            'x509_authenticate': False,
            'passthrough_authenticate': False,
            'password': 'internalpass',
            'preferredLanguage': u'en',
            'rights':
            [
                'itnl_internal@bouncer2:RDP',
                'itnl_internal@test_card:RDP',
                'itnl_internal@widget2_message:RDP',
                'itnl_replay@autotest:RDP',
                'itnl_internal@widgettest:RDP',
            ]
        },
        'rec':
        {
            'is_x509_connected': False,
            'x509_authenticate': False,
            'passthrough_authenticate': False,
            'password': 'recpass',
            'preferredLanguage': u'en',
            'rights':
            [
                'rec_w2k_administrateur@10.10.46.64:RDP',
                'rec_w2k_administrateur@10.10.47.39:RDP',
                'rec_w2k3_administrateur@10.10.47.205:RDP',
                'rec_w2k8_administrateur@10.10.47.89:RDP',
                'rec_w2k3_any@10.10.46.70:VNC',
                'rec_w2k3_qa\\administrateur@10.10.46.70:RDP',
                'rec_w2k8_qa\\administrateur@10.10.46.78:RDP',
                'rec_wxp_administrateur@10.10.47.175:RDP',
                'rec_w2k12_administrateur@10.10.47.132:RDP'
            ]
        },
        'x':
        {
            'is_x509_connected': False,
            'x509_authenticate': False,
            'passthrough_authenticate': False,
            'password': 'xpass',
            'preferredLanguage': u'en',
            'rights':
            [
                'w2k_administrateur@10.10.46.64:RDP',
                'w2k_administrateur@10.10.47.39:RDP',
                'w2k3_administrateur@10.10.47.205:RDP',
                'w2k3_any@10.10.42.13:VNC',
                'w2k3_any@10.10.46.70:VNC',
                'w2k3_qa\\administrateur@10.10.46.70:RDP',
                'w2k8_administrateur@10.10.47.89:RDP',
                'w2k8_qa\\administrateur@10.10.46.78:RDP',
                'w2k8_qa\\administrateur@10.10.46.88:RDP',
                'wxp_administrateur@10.10.47.175:RDP',
                'w2k8erpm\\administrateur@10.10.46.110:RDP',
                'w2k8erpm\\special@10.10.46.110:RDP'
            ]
        },
        'fr':
        {
            'is_x509_connected': False,
            'x509_authenticate': False,
            'passthrough_authenticate': False,
            'password': 'frpass',
            'preferredLanguage': u'fr',
            'rights':
            [
                'w2k_administrateur@10.10.46.64:RDP',
                'w2k3_any@10.10.46.70:VNC',
                'w2k8_qa\\administrateur@10.10.46.78:RDP',
                'w2k8_qa\\administrateur@10.10.46.88:RDP'
            ]
        },
        'challenge':
        {
            'is_x509_connected': False,
            'x509_authenticate': False,
            'passthrough_authenticate': False,
            'password': 'challengepass',
            'response': 'yes',
            'preferredLanguage': u'en',
            'rights':
            [
                'w2k_administrateur@10.10.46.64:RDP',
                'w2k3_any@10.10.46.70:VNC',
                'w2k8_qa\\administrateur@10.10.46.78:RDP',
                'w2k8_qa\\administrateur@10.10.46.88:RDP'
            ]
        },
        'x509':
        {
            'is_x509_connected': True,
            'x509_authenticate': True,
            'passthrough_authenticate': False,
            'password': 'x509pass',
            'preferredLanguage': u'en',
            'rights':
            [
                'w2k8_administrateur@qa@10.10.146.78:RDP',
                'w2k8_qa\\administrateur@10.10.46.78:RDP'
            ]
        }
    }

    def __init__(self):
        class User:
            def __init__(self, preferredLanguage = u'en'):
                self.preferredLanguage = preferredLanguage
                self.cn = None
        Logger().info("Engine constructor")
        self.wab_login = None
        self.user = User()
        self._trace_encryption = False
        self.challenge = None

    def get_trace_encryption(self):
        return self._trace_encryption

    def password_expiration_date(self):
        #return false or number of days
        return False, 0

    def is_x509_connected(self, wab_login, ip_client, proxy_type, target, server_ip):
        """
        Ask if we are authentifying using x509
        (and ask user by opening confirmation popup if we are,
        session ticket will be asked later in x509_authenticate)
        """
        print ('is_x509_connected(%s %s %s %s)' % (wab_login, ip_client, proxy_type, target))
        res = False
        try:
            res = self.config_users[wab_login]['is_x509_connected']
            if res:
                self.wab_login = wab_login
        except:
            pass
        return res

    def x509_authenticate(self):
        print ('x509_authenticate(%s)' % self.wab_login)
        try:
            if self.config_users[self.wab_login]['x509_authenticate']:
                self.user.preferredLanguage = self.config_users[self.wab_login][u'preferredLanguage']
                return True
            return False
        except:
            self.wab_login = None
        return False

    def password_authenticate(self, wab_login, ip_client, password, server_ip):
        print ('password_authenticate(%s %s %s)' % (wab_login, ip_client, password))
        res = False
        try:
            print ('::::: %s' % (self.config_users[wab_login]))
            if wab_login == 'challenge':
                res = self.challenge_manage(wab_login, password)
            else:
                res = self.config_users[wab_login]['password'] == password
            if res:
                self.wab_login = wab_login
                self.user.preferredLanguage = self.config_users[wab_login][u'preferredLanguage']
        except:
            pass
        return res

    def passthrough_authenticate(self, wab_login, ip_client, server_ip):
        print ('passthrough_authenticate(%s %s)' % (wab_login, ip_client))
        try:
            if self.config_users[self.wab_login]['passthrough_authenticate']:
                self.user.preferredLanguage = self.config_users[self.wab_login][u'preferredLanguage']
                return True
            return False
        except:
            self.wab_login = None
        return False

    def get_license_status(self):
        return True

    def challenge_manage(self, wab_login, password):
        res = False
        if not self.challenge:
            res = self.config_users[wab_login]['password'] == password
            if res:
                self.challenge = ChallengeInfo("""When Littlefoot's Mother died in the original
'Land Before Time', did you feel sad ?
(Bots: No lying)""",
                                               False)
                res = False
        else:
            res = self.config_users[wab_login]['response'] == password
            self.challenge = None
        return res



    def NotifyConnectionToCriticalEquipment(self, protocol, user, source,
            ip_source, login, device, ip, time, url):
        notif_data = {
               u'protocol' : protocol
             , u'user'     : user
             , u'source'   : source
             , u'ip_source': ip_source
             , u'login'    : login
             , u'device'   : device
             , u'ip'       : ip
             , u'time'     : time
         }

        if not (url is None):
            notif_data[u'url'] = url

        Logger().info(u"NotifyConnectionToCriticalEquipment: %r" % notif_data)

    def NotifyPrimaryConnectionFailed(self, user, ip):
        notif_data = {
               u'user' : user
             , u'ip'   : ip
         }

        Logger().info(u"NotifyPrimaryConnectionFailed: %r" % notif_data)

    def NotifySecondaryConnectionFailed(self, user, ip, account, device):
        notif_data = {
               u'user'   : user
             , u'ip'     : ip
             , u'account': account
             , u'device' : device
         }

        Logger().info(u"NotifySecondaryConnectionFailed: %r" % notif_data)

    def NotifyFilesystemIsFullOrUsedAtXPercent(self, filesystem, used):
        notif_data = {
               u'filesystem' : filesystem
             , u'used'       : used
         }

        Logger().info(u"NotifyFilesystemIsFullOrUsedAtXPercent: %r" % notif_data)

    def NotifyFindPatternInRDPFlow(self, regexp, string, user_login, user, host, cn):
        notif_data = {
               u'regexp'     : regexp
             , u'string'     : string
             , u'user_login' : user_login
             , u'user'       : user
             , u'host'       : host
             , u'device'     : cn
         }

        Logger().info(u"NotifyFindPatternInRDPFlow: %r" % notif_data)

    def get_proxy_rights(self, protocols):
        self.rights = [ self.config_rights[r] for r in self.config_users[self.wab_login]['rights']]
        return self.rights

    def get_selected_target(self, target_device, target_login, target_protocol):
        selected_target = None
        for r in self.rights:
            if r.resource.application:
                if target_device != r.resource.application.cn:
                    continue
                if target_login != r.account.login:
                    continue
                if target_protocol != u'APP':
                    continue
            else:
                if target_device != r.resource.device.cn:
                    continue
                if target_login != r.account.login:
                    continue
                if target_protocol != r.resource.service.cn:
                    continue
            selected_target = r
            break

        return selected_target

    def get_effective_target(self, selected_target):
        service_login = selected_target.service_login
        Logger().info("Effective_target %s" % service_login)
        try:
            if selected_target.resource.application:
                res = [ self.config_effective_targets[r] for r in self.config_service_logins[service_login]['rights']]
                Logger().info("Engine get_effective_target done (application)")
                return effective_target
            else:
                Logger().info("Engine get_effective_target done (physical)")
                return [selected_target]

        except Exception, e:
            import traceback
            Logger().info("%s" % traceback.format_exc(e))
#        Logger().info("Effective_target ok %r" % res)
        return res

    def get_app_params(self, selected_target, effective_target):
        service_login = selected_target.service_login
        res = None
#        Logger().info("get_app_params %s" % service_login)
        try:
            for key, value in self.config_effective_targets.items():
#                Logger().info('key=%s value=%s' % (key, value))
                if value  is effective_target:
                    res = self.config_app_params[service_login][key]
        except Exception, e:
            import traceback
            Logger().info("%s" % traceback.format_exc(e))
#        Logger().info("get_app_params done = %s" % res)
        return res

    def get_target_password(self, target_device):
        res = None
        Logger().info("get_target_password ...")
        Logger().info("account =%s" % target_device.account.login)
        Logger().info("resource=%s" % target_device.resource.device.cn)
        Logger().info("protocol=%s" % target_device.resource.service.protocol.cn)
        try:
            for p in self.config_target_password:
                if (p.account == target_device.account.login and
                   p.resource == target_device.resource.device.cn and
                   p.protocol == target_device.resource.service.protocol.cn):
                    res = p.password
        except Exception, e:
            import traceback
            Logger().info("%s" % traceback.format_exc(e))
        Logger().info("get_target_password done = %s" % res)
        return res

    def release_target_password(self, target_device, reason, target_application = None):
        Logger().info("release_target_password done: target_device=\"%s\" reason=\"%s\"" %
            (target_device, reason))

    def start_session(self, target, pid, effective_login):
        return "SESSIONID-0000"

    def get_restrictions(self, target):
        self.pattern_kill = u""
        self.pattern_notify = u""
        return

    def update_session(self, target31):
        pass

    def stop_session(self, result=True, diag=u"success", title=u"End session"):
        pass

    def write_trace(self, video_path):
        return True, ""

    def get_targets_list(self, group_filter, device_filter, protocol_filter,
                         real_target_device):
        targets = []
        item_filtered = False
        for right in self.rights:
            if not right.resource.application:
                if (right.resource.device.host == u'autotest' or
                    right.resource.device.host == u'bouncer2' or
                    right.resource.device.host == u'widget2_message' or
                    right.resource.device.host == u'widgettest' or
                    right.resource.device.host == u'test_card'):
                    temp_service_login                = right.service_login.replace(u':RDP', u':INTERNAL', 1)
                    temp_resource_service_protocol_cn = 'INTERNAL'
                    temp_resource_device_cn           = right.resource.device.cn
                else:
                    temp_service_login                = right.service_login
                    temp_resource_service_protocol_cn = right.resource.service.protocol.cn
                    temp_resource_device_cn           = right.resource.device.cn
            else:
                temp_service_login                = right.service_login + u':APP'
                temp_resource_service_protocol_cn = u'APP'
                temp_resource_device_cn           = right.resource.application.cn

            if ((right.target_groups.find(group_filter) == -1)
                or (temp_service_login.find(device_filter) == -1)
                or (temp_resource_service_protocol_cn.find(protocol_filter) == -1)):
                item_filtered = True
                continue

            if real_target_device:
                if right.resource.application:
                    continue
                if (right.resource.device
                    and (not is_device_in_subnet(real_target_device,
                                                 right.resource.device.host))):
                    continue

            targets.append((right.target_groups # ( = concatenated list)
                            , temp_service_login
                            , temp_resource_service_protocol_cn
                            , (right.deconnection_time if right.deconnection_time[0:4] < "2034" else u"-")
                            , temp_resource_device_cn
                            )
                           )
        return targets, item_filtered
