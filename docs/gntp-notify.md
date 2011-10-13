% GNTP-NOTIFY(1) GNTP User Manuals | Version 1.0
% Al Payne
% October 12, 2011

# NAME

gntp-notify - Growl Notification Transport Protocol notifier

# SYNOPSIS

gntp-notify [-s] [-H _host_[:_port_]] [-P _password_] [-p _priority_] -a _application_ -n _notification_ -t _title_ -m _message_ 

# DESCRIPTION

Send notification messages to Growl.  Applications must register (see gntp-register) with Growl before notifications will be displayed. 


# OPTIONS

-a _application_
:   Name of the application sending the notice.  Must match a previously registered application name.

-H _host_[:_port_]
:	Host to send the notification message to.  Defaults to localhost if blank.  Port may also be defined if using a non-standard listening port on the client.

-m _message_
:   The message text to be displayed on the Growl client.

-n _notification_
:   Name of the notification as defined in Growl. Must match a previously registered notification name.

-P _password_
:   Password of the receiving Growl client.

-p _priority_
:   Optional priority level for the message.  A higher number indicates a higher priority. Valid values are between –2 and 2, with a default of 0.  In Growl this translates to "Very Low", "Moderate", "Normal", "High", "Emergency".   This is a display hint for the Growl display which may be ignored.

-s
:   Display hint asking the Growl display to make the message sticky (stay up until clicked). May be ignored. 

-t _title_
:   The title of the message to be displayed on the Growl client.

# STANDARDS

gntp-notify sends Growl Notification Transfer Protocol version 1.0 transactions.

# SECURITY CONSIDERATIONS

gntp-notify does not used GNTP encryption.  Users should be aware that any information sent in the notification is transmitted in the clear.

# HISTORY

gntp-notify was originall based on mattn's gntp-send.

# AUTHOR

Al Payne  <alpayne@pleiades.com>

# COPYRIGHT

Copyright 2011 - Al Payne - Licensed under the BSD license

# SEE ALSO

gntp-register
