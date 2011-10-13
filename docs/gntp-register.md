% GNTP-REGISTER(1) GNTP User Manuals | Version 1.0
% Al Payne
% October 12, 2011

# NAME

gntp-register - send Growl Notification Transport Protocol register requests

# SYNOPSIS

gntp-notify [-s] [-H _host_[:_port_]] [-P _password_] [-p _priority_] -a _application_ -n _notification_ -t _title_ -m _message_ 

# DESCRIPTION

gntp-register registers applications and notifications with Growl to enable gntp-notify messages to be displayed.

Applications are message source.  Typically they are identified in Growl by the application's common name.  Since gntp-notify sends notifications on behalf of other processes, this name can be any descriptive name identifying the message source.

Notifications in Growl identify the class of message being sent.  They are short names such as "File updated", or "New message".  From a Growl user perspective, notifications allow more granular format control for messages.  Multiple notification names can be registered.  

# OPTIONS

-a _application_
:   Name of the application to register.

-H _host_[:_port_]
:	Host to send the notification message to.  Defaults to localhost if blank.  Port may also be defined if using a non-standard listening port on the client.

-n _notification_
:   Name of the notification to register.  Multiple notifications may be registered using multiple -n options.

-P _password_
:   Password of the receiving Growl client.

# STANDARDS

gntp-register sends Growl Notification Transfer Protocol version 1.0 transactions.

# SECURITY CONSIDERATIONS

gntp-register does not used GNTP encryption.  Users should be aware that any information sent in the notification is transmitted in the clear.

# AUTHOR

Al Payne  <alpayne@pleiades.com>

# COPYRIGHT

BSD license

# SEE ALSO

gntp-notify
