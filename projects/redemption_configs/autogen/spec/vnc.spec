[general]

# Secondary login Transformation rule
# ${LOGIN} will be replaced by login
# ${DOMAIN} (optional) will be replaced by domain if it exists.
# Empty value means no transformation rule.
transformation_rule = string(default="")

# Account Mapping password retriever
# Transformation to apply to find the correct account.
# ${USER} will be replaced by the user's login.
# ${DOMAIN} will be replaced by the user's domain (in case of LDAP mapping).
# ${USER_DOMAIN} will be replaced by the user's login + "@" + user's domain (or just user's login if there's no domain).
# ${GROUP} will be replaced by the authorization's user group.
# ${DEVICE} will be replaced by the device's name.
# A regular expression is allowed to transform a variable, with the syntax: ${USER:/regex/replacement}, groups can be captured with parentheses and used with \1, \2, ...
# For example to replace leading "A" by "B" in the username: ${USER:/^A/B}
# Empty value means no transformation rule.
vault_transformation_rule = string(default="")

[session]

# No automatic disconnection due to inactivity, timer is set on target session.
# If value is between 1 and 30, then 30 is used.
# If value is set to 0, then value set in "Base inactivity timeout" (in "RDP Proxy" configuration option) is used.<br/>
# (in seconds)
inactivity_timeout = integer(min=0, default=0)

[vnc]

support_cursor_pseudo_encoding = boolean(default=True)

#_display_name=Server is MacOS
server_is_macos = boolean(default=False)

# When disabled, Ctrl + Alt becomes AltGr (Windows behavior)
#_display_name=Server Unix alt
server_unix_alt = boolean(default=False)

# Enable target connection on IPv6
#_display_name=Enable IPv6
enable_ipv6 = boolean(default=True)

[vnc_over_ssh]

enable = boolean(default=False)

# Port to be used for SSH tunneling
#_display_name=SSH port
ssh_port = integer(min=0, default=22)

# static_login: Static values provided in "Ssh login" &amp; "Ssh password" fields will be used to establish the SSH tunnel.
# scenario_account: Scenario account provided in "Scenario account name" field will be used to establish the SSH tunnel. (Recommended)
tunneling_credential_source = option('static_login', 'scenario_account', default="scenario_account")

# Login to be used for SSH tunneling.
#_display_name=SSH login
ssh_login = string(default="")

# Password to be used for SSH tunneling.
#_display_name=SSH password
ssh_password = string(default="")

# With the following syntax: "account_name@domain_name[@[device_name]]".<br/>
# Syntax for using global domain scenario account:
# &nbsp; &nbsp;   "account_name@global_domain_name"<br/>
# Syntax for using local domain scenario account (with automatic device name deduction):
# &nbsp; &nbsp;   "account_name@local_domain_name@"
scenario_account_name = string(default="")

# Only for debugging purposes.
#_advanced
tunneling_type = option('pxssh', 'pexpect', 'popen', default="pxssh")

