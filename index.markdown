---
title: alpayne/GNTP-Tools @ GitHub
tagline: Send notification messages over GNTP to Growl from OSX, Linux, or Windows
pkglicense:  GNTP-Tools is licensed under the BSD License. &copy; 2011, Al Payne
license: This page is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by/3.0/">Creative Commons Attribution 3.0 Unported License</a>.
layout: default
---
GNTP is a protocol for sending notification messages to Growl. GNTP-Tools is a pair of command line utilities for sending messages from Linux, OS X, and Windows hosts to remote machines running [Growl](http://growl.info) and [Growl for Windows](http://www.growlforwindows.com/gfw/).

Before Growl will display incoming notifications, the application must first be registered using ```gntp-register```. The registration message sets the application's name and the notifications it may send.  Notifications that don't correspond to previously registered application/notification pairs will not be displayed.

After registering, the application may send _notify_ requests with ```gntp-notify``` to be displayed by Growl. 

## Downloads

Binaries can be found in the project [binaries directory](https://github.com/alpayne/Growl-GNTP-Tools/tree/master/binaries) on Github.

The source code can be cloned from GitHub : [alpayne/Growl-GNTP-Tools](http://github.com/alpayne/Growl-GNTP-Tools), 
or downloaded as a [zip](http://github.com/alpayne/Growl-GNTP-Tools/zipball/master) or
[tar](http://github.com/alpayne/Growl-GNTP-Tools/tarball/master) archive.

A markdown verion of the [GNTP v1.0 specification](https://github.com/alpayne/Growl-GNTP-Tools/blob/master/docs/GNTP-v1.0.md) is also in the repository. This version has been edited to reorder some sections and make some minor corrections.

## Man pages

Man pages are available with the binaries above, or online:

* [gntp-register](https://github.com/alpayne/Growl-GNTP-Tools/blob/master/docs/gntp-register.md)
* [gntp-notify](https://github.com/alpayne/Growl-GNTP-Tools/blob/master/docs/gntp-notify.md)

## Example

To register the application and its notifications:

    gntp-register -H 192.168.0.1 -P password -a "GNTP Tools - Demo" \
				  -n "DemoNotification1" -n "DemoNotification2" 

On the Growl client, the application should appear in the list under the name given in the '-a' option.

![](images/demo/applications.png)

Click configure, and the settings window will appear.  The notifications tab shows what we defined in the '-n' options.

![](images/demo/notifications.png)


Notes:

1. The Stay On Screen option can be set to Never, Always, or Application Decides.  If left on Application Decides, the '-s' option will make the message stay on the screen.  The other two options will override '-s'.
2. A default priority can be defined, and then overridden with the '-p' option in gntp-notify. Depending on the display chosen, the priority may or may not have any effect on the look of the message.

Sending a notification:


    gntp-notify -H 192.168.0.1 -P password -a "GNTP Tools - Demo" -n DemoNotification1 \
                -t "Notify title" -m "Message body.  Anything goes here" 


You should see a message displayed on the screen, similar to:

![](images/demo/displayed.png)

