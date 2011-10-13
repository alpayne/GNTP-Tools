# Growl Notification Transport Protocol (GNTP) - v1.0

## Introduction
The Growl Notification Transport Protocol, or GNTP, is a protocol to allow two-way communication between applications and centralized notification systems such as Growl for Mac OS X or Growl for Windows, and to allow two-way communication between two machines running centralized notification systems for notification forwarding purposes.

## Communication Overview

An application MUST first REGISTER with the notification system. This registration will communicate the application's name, the notifications it may send, and details on those notifications. Requests to NOTIFY with information not corresponding to a previously registered application/notification pair will be ignored.

After registering, the application may at any time send NOTIFY requests. A NOTIFY request includes the information to be displayed, such as title, text, and icon of the notification, and may also include hints to the display server, as detailed below.

## General Message Structure

GNTP messages follow a structured plain text format with lines separated by \<CRLF\>, similar to MIME headers.

### GNTP Information Line

The first line of the message MUST include the protocol identifier, version, message type, encryption algorithm id, and optionally, the key hash algorithm id, key hash, and salt:

    GNTP<version> <messagetype> <encryptionAlgorithmID>[:<ivValue>][<keyHashAlgorithmID>:<keyHash>.<salt>]

where GNTP is the name of the protocol and:

\<version\>
:   The GNTP version number. As of this document's publication, the only supported version is '1.0'.

\<messagetype\>
:   Identifies the type of message. See Requests and Responses sections below for supported values.

\<encryptionAlgorithmID\>
:   Identifies the type of encryption used on the message. See Encrypting Messages section below for supported values.

\<ivValue\>
:   If the message is encrypted, this is the hex-encoded initialization value.

\<keyHashAlgorithmID\>
:   Identifies the type of hashing algorithm used. See Encrypting Messages section below for supported values.

\<keyHash\>
:   The hex-encoded hash of the key.

\<salt\>
:   The hex-encoded salt value.

**NOTE:** \<keyHashAlgorithm\>, \<keyHash\>, and \<salt\> are not normally required for requests that originate on the local machine.

The GNTP information line is followed by a \<CRLF\> and then any message headers.

### Header Value Data Types

Message headers MUST be in the format:

    Header-Name: value

Each header name-value pair is terminated with \<CRLF\>. Neither the header name nor value may contain the \<CRLF\> character sequence.

Header values may be any of the following types:

\<string\>
:   Any string EXCEPT one that contains the \<CRLF\> character sequence. If a newline must be passed in a value, use just the \<LF\> character ('\n' in a C-string). All strings should be encoded using UTF-8 encoding.

\<int\>
:   Any integer value. Some headers may place additional restrictions on the range of valid values.

\<boolean\>
:   A value that indicates True or False. [Yes|True] | [No|False]

\<date\>
:   A date/time value conforming to the ISO 8601 standard with the following format: yyyy-MM-dd mm:hh:ssZ (e.g.: 2008-08-09 22:21:00Z)

\<url\>
:   A valid url value

\<uniqueid\>
:   A special pointer that indicates additional binary data will be appended to the messages. \<uniqueid\> values MUST be in the following format:

        x-growl-resource://<uniquevalue>

For example:

        Notification-Icon: x-growl-resource://6027F6C0-64AF-11DD-9779-EEDA55D89593

\<uniquevalue\>
:   could be the MD5 hash of the data, but can be any unique value. The server MAY choose to use a cached copy of data with the same uniqueid, so MD5 is an excellent choice. It may do this as a bandwidth conserving measure, as it may then interrupt packets before they are fully received if all uncached data has been received.

Each \<uniqueid\> value will correspond to a binary section which follows all other data in the request. Each binary section should start with the 'Identifier' header/value, followed by a 'Length' header/value:  

    Identifier: 6027F6C0-64AF-11DD-9779-EEDA55D89593
    Length: 1048

where the value of 'Length' is the number of bytes of data. The 'Length' header should be followed by an empty \<CRLF\> line and then the binary data bytes, followed by another empty \<CRLF\> line.

### Message Termination

A GNTP request must end with \<CRLF\>\<CRLF\>. This indicates the end of the request to the receiver.

An additional end-of-message semaphore is also required in certain cases. See the Connection Handling section below for more information.

## Requests

As of v1.0, the only allowed values for \<messagetype\> in requests are:

* REGISTER
* NOTIFY
* SUBSCRIBE

For each message type, the following headers are defined:

### REGISTER 

Application-Name: \<string\>
:   Required - The name of the application that is registering.  Future NOTIFY messages for this application must use this exact name.

Application-Icon: \<url\> | \<uniqueid\>
:   _Optional_ - The icon of the application.

Notifications-Count: \<int\>
:   Required - The number of notifications being registered.

For each notification being registered:

\<blank line\>
:   Required - Each notification should be preceded by a blank line.

Notification-Name: \<string\>
:   Required - The name (type) of the notification being registered.  This is the label that will appear in the notification customization tabs in the Growl client.

Notification-Display-Name: \<string\>
:   _Optional_ - The name of the notification that is displayed to the user (defaults to the same value as Notification-Name).

Notification-Enabled: \<boolean\>
:   _Optional_ - Indicates if the notification should be enabled by default (defaults to False).

Notification-Icon: \<url\> | \<uniqueid\>
:   _Optional_ - The default icon to use for notifications of this type.


### NOTIFY 

Application-Name: \<string\>
:   Required - The name of the application that sending the notification (must match a previously registered application).

Notification-Name: \<string\>
:   Required - The name (type) of the notification (must match a previously registered notification name registered by the application specified in Application-Name).

Notification-ID: \<string\>
:   _Optional_ - A unique ID for the notification. If used, this should be unique for every request, even if the notification is replacing a current notification (see _Notification-Coalescing-ID_).

Notification-Title: \<string\>
:   Required - The notification's title.

Notification-Text: \<string\>
:   _Optional_ - The notification's text (defaults to "").

Notification-Sticky: \<boolean\>
:   _Optional_ - Indicates if the notification should remain displayed until dismissed by the user (default to False).

Notification-Priority: \<int\>
:   _Optional_ - A higher number indicates a higher priority. This is a display hint for the receiver which may be ignored (valid values are between -2 and 2, defaults to 0).

Notification-Icon: \<url\> | \<uniqueid\>
:   _Optional_ - The icon to display with the notification.

Notification-Coalescing-ID: \<string\>
:   _Optional_ - If present, should contain the value of the Notification-ID header of a previously-sent notification. This serves as a hint to the notification system that this notification should replace/update the matching previous notification. The notification system may ignore this hint.

Notification-Callback-Context: \<string\>
:   _Optional_ - Any data (will be passed back in the callback unmodified).

Notification-Callback-Context-Type: \<string\>
:   _Optional_, but Required if 'Notification-Callback-Context' is passed - The type of data being passed in Notification-Callback-Context (will be passed back in the callback unmodified). This does not need to be of any pre-defined type, it is only a convenience to the sending application.

Notification-Callback-Target: \<string\>
:   _Optional_ - An alternate target for callbacks from this notification. If passed, the standard behavior of performing the callback over the original socket will be ignored and the callback data will be passed to this target instead. See the URL Callbacks section for more information.

### SUBSCRIBE 

Subscriber-ID: \<string\>
:   Required - A unique id (UUID) that identifies the subscriber.

Subscriber-Name: \<string\>
:   Required - The friendly name of the subscribing machine.

Subscriber-Port: \<int\>
:   _Optional_ - The port that the subscriber will listen for notifications on (defaults to the standard 23053).

Subscription requests MUST include the key hash (In practice, only instances subscribing from another machine make sense, so they would have to provide the key hash anyway since they are communicating over the network.)

Once the subscription is made, the subscribed-to machine should forward requests to the subscriber, just as if the end user had configured the subscriber using the standard forwarding mechanisms, with one important distinction: the forwarding machine should use a specially-constructed password consisting of -its own- password plus the Subscriber-ID value when constructing the forwarded messages (as opposed to the usual behavior of using the receiver's password to construct the message - see example below). This is due to the fact that the subscribed-to machine does not know the subscriber's password (but the subscriber must know/provide the subscribed-to machine's password when subscribing). Note that the subscriber should be prepared to accept incoming requests using this password for the lifetime of the subscription. This also ensures that if the password is changed on the subscribed-to machine, the subscribed machine can no longer receive subscribed notifications.

#### Example:

Subscribed-to Machine's password: `foo`

Subscriber-ID value sent by subscribing machine: `0f8e3530-7a29-11df-93f2-0800200c9a66`

Resulting password used by subscribed-to machine when forwarding: `foo0f8e3530-7a29-11df-93f2-0800200c9a66`

If the subscriber wants to remain subscribed, they must issue another SUBSCRIBE request before the TTL period has elapsed. It is recommended that the subscribed-to machine not set the TTL to less than 60 seconds to give the subscriber time to process notifications and issue their renewal requests.

If the subscriber does not issue another SUBSCRIBE request before the TTL period has elapsed, the subscribed-to machine should stop forwarding requests to the subscriber.

### Generic Headers

For all request types, the following generic headers may also be sent:

Origin-Machine-Name: \<string\>
:   _Optional_ - The machine name/host name of the sending computer

Origin-Software-Name: \<string\>
:   _Optional_ - The identity of the sending framework. Example: GrowlAIRConnector

Origin-Software-Version: \<string\>
:   _Optional_ - The version of the sending framework. Example: 1.2

Origin-Platform-Name: \<string\>
:   _Optional_ - The identify of the sending computer OS/platform. Example: Mac OS X

Origin-Platform-Version: \<string\>
:   _Optional_ - The version of the the sending computer OS/platform. Example: 10.6


### Custom Headers

In addition to the defined headers listed above, custom headers may be included and MUST start with 'X-':

`X-Sender: PHP GNTP Library`

Custom headers may link to binary content using the \<uniqueid\> syntax. To accomodate this, an application cannot send a custom header that contains the value of a literal string that starts with 'x-growl-resource:' (there would be no way to differentiate the intent)

Custom X-* headers are used to extend the GNTP spec without waiting for headers to become officially rolled into the spec. If instead you simply want to pass some extra app-specific data with your request which will be returned to you in any response, see the App-Specific Headers section.

If you are using custom headers in a GNTP implementation, please email the Growl team with the information. We will document such headers here to allow standardization (and possible official inclusion in a future version of the protocol).

Known custom headers:

`X-Application-BundleID: <string>`
:   Growl (Mac OS X) - Used for application registration to identify the registering bundle ID, which is in reverse domain name notation.

### App-Specific Headers

In addition to the defined headers listed above, an application can also pass arbitrary data in a GNTP request and that data will be returned in any response to that request. App-specific data headers are optional and MUST start with 'Data-':

`Data-Filename: file.txt`

App-specific data headers may link to binary content using the \<uniqueid\> syntax. To accomodate this, an application cannot send a data header that contains the value of a literal string that starts with 'x-growl-resource:' (there would be no way to differentiate the intent)

Data-* headers sent in a GNTP request will be echoed back in the -OK and -CALLBACK responses. -ERROR responses will not echo back the Data-* headers (since they may not have been parsed properly).



## Connection Handling


The default behavior for GNTP messages is for each request-response-callback combination to take place on a seperate socket connection. The sequence looks like this:

1. Sending application opens a new socket
2. Sending application sends request message
3. Notification system receives request, handles it, and returns the response message
4. Sending application receives the response. If no callback was specified, the socket connection is closed
5. If a callback was requested, the notification system will use the original socket to return the callback message
6. Sending application receive the callback and the socket connection is closed
7. Each new request repeats this process

Some applications may wish to reuse sockets instead of creating a new socket connection for each request. For this scenario, all request messages support an additional header:

Connection: \<[Close|Keep-Alive]\>
:   _Optional_ - If set to 'Keep-Alive', the socket connection can be reused by the sending application. Defaults to 'Close'. Note that the default behavior is the opposite of the way HTTP handles requets.

If the 'Connection' header is specified as 'Keep-Alive', all GNTP messages (requests, responses, and callbacks) must also include an end-of-message semaphore. After all message headers and any inline binary data, the following character sequence should be sent immediately before the usual GNTP message terminator (two \<CRLF\> sequences):

    GNTP/<version> END

When reusing sockets, the sending application will be responsible for keeping track of which responses and callbacks are associated with which request. There are also important performance considerations to be aware of when choosing whether to reuse sockets. When sending requests with no inline binary data over high-latency connections, socket reuse can reduce the time it would have taken to renegotiate each connection. However, the receiving notification system is no longer able to read partial requests, so any inline binary data will never be eligible to be read from cache, significantly increasing the request transmission time.

**NOTE:** Growl for Windows v2.0 does not yet support the 'Connection' header, socket reuse, or the GNTP/1.0 END semaphore.

## Responses

As with requests, the first line of a GNTP response must MUST include the protocol identifier, version, message type, and encryption algorithm id. However, the key information is never included.

As of v1.0, the only allowed values for \<messagetype\> in responses are:

* -OK 
* -ERROR

Responses to successful requests should be encrypted using the same encryption algorithm and key specified in the request.

**NOTE:** -ERROR responses will always have the \<encryptionAlgorithmID\> set to NONE and will not be encrypted (since the error may have been due to a missing encryption algorithm id or not being able to decrypt the message, etc).

Growl for Windows currently always returns responses in plain text, regardless of the encryption algorithm used in the request. For each response message type, the following headers are defined:

### -OK

The response MUST include a header which indicates the \<messagetype\> to which it is responding, e.g.:

Response-Action: REGISTER
:   Additionally, responses to each \<messagetype\> should contain the following headers:

If the request was a REGISTER request:

[no headers required, but informational headers may be returned]

If the request was a NOTIFY request:

Notification-ID: \<string\>
:   Required - The value of the 'Notification-ID' header from the request (even if it was an empty string)

If the request was a SUBSCRIBE request:

Subscription-TTL: \<int\>
:   Required - The number of seconds the subscription is valid for

### -ERROR

Error-Code: \<int\>
:   See list of possible values in Error Code values appendix below.

Error-Description: \<string\>
:   A textual description of the error that occurred.

A request may result in an -ERROR response for many reasons, including (but not limited to):

  * The request was not able to be sent because of network-related issues (host not available, socket prematurely closed, etc)
  * The request is malformed, uses an unsupported GNTP protocol version, is missing required data, or is otherwise invalid
  * Is a NOTIFY request, but the application is not yet registered, or has not registered the corresponding notification type
  * The request requires a valid key but the supplied key information is missing or does not match

### Generic Headers

For all types of responses, the following generic headers may also be returned:

Origin-Machine-Name: \<string\>
:   _Optional_ - The machine name/host name of the responding computer

Origin-Software-Name: \<string\>
:   _Optional_ - The identity of the responding framework. Example: GrowlAIRConnector

Origin-Software-Version: \<string\>
:   _Optional_ - The version of the responding framework. Example: 1.2

Origin-Platform-Name: \<string\>
:   _Optional_ - The identify of the responding computer OS/platform. Example: Mac OS X

Origin-Platform-Version: \<string\>
:   _Optional_ - The version of the the responding computer OS/platform. Example: 10.6

### Custom Headers

All response types may also contain custom headers as defined in the 'Custom Headers' portion of the 'Requests' section above.

### App-Specific Headers

Any app-specific (Data-*) headers received in a GNTP request will be echoed back in the -OK and -CALLBACK responses as outlined in the 'App-Specific Headers' portion of the 'Requests' section above. -ERROR responses will not echo back the Data-* headers.

## Callbacks

Callbacks are sent back to the sending application when an action is taken in response to a notification. Callbacks are formatted similar to responses, but the only supported \<messagetype\> is:

  * -CALLBACK

If the NOTIFY message contains the 'Notification-Callback-Context' and 'Notification-Callback-Context-Type' headers, the notification system will send a callback when a callback condition is met. Callback conditions are:

  * The notification is clicked by the end user
  * The notification is closed explicitly by the end user
  * The notification times out

Only ONE of these three conditions - whichever occurs first - will trigger a callback for any given notification.

A callback may be sent either via the TCP socket which made the connection initially or to an alternative URL specified in the 'Notification-Callback-Target' header of the request.

### Socket Callbacks

If the NOTIFY request contains the 'Notification-Callback-Context' and 'Notification-Callback-Context-Type' headers *and* the 'Notification-Callback-Target' header is not specified, then the receiver will maintain the TCP connection until a callback condition occurs. The callback MUST contain the following headers:

Application-Name: \<string\>
:   Required - The name of the application that sent the original request

Notification-ID: \<string\>
:   Required - The value of the 'Notification-ID' header from the original request

Notification-Callback-Result: \<string\>
:   Required - [CLICKED|CLOSED|TIMEDOUT] | [CLICK|CLOSE|TIMEOUT]

Notification-Callback-Timestamp: \<date\>
:   Required - The date and time the callback occurred

Notification-Callback-Context: \<string\>
:   Required - The value of the 'Notification-Callback-Context' header from the original request

Notification-Callback-Context-Type: \<string\>
:   Required - The value of the 'Notification-Callback-Context-Type' header from the original request

Callbacks may also contain the generic headers defined in the 'Responses' section, as well as custom headers as defined in the 'Custom Headers' portion of the 'Requests' section above.

### URL Callbacks 

If a URL is specified via the 'Notification-Callback-Target' header, the TCP connection will be closed once a notification is received. When a notification is clicked, the callback URL will be opened in the user's default browser. Unlike socket callbacks, URL callbacks are only triggered if the notification is clicked (CLICK|CLICKED), not for CLOSE|CLOSED or TIMEOUT|TIMEDOUT actions. The callback URL is not modified in any way, so if notification information is required (such as Notification ID), it must be specified as part of the callback URL in the request.

Notes:

  * A notification can forgo callback notifications by omitting all of the 'Notification-Callback-*' headers.

## Forwarding

If a message is forwarded by a notification system, it MUST add a 'Received' header in the following format:

    Received: From <hostname> by <hostname> [with Growl] [id <identifier>]; <date>

Every 'Received' header will be propagated through any number of forwarded messages.

For forwarded messages requiring socket callback notification, each forwarding daemon should keep the connection open until the notification is clicked/closed/times out. Once the event occurs, the callback response is returned back up the forwarding chain to the original application (each return daemon should add the 'Received' header to the response as well). If the notification has already been acted upon in a previous context, further callbacks will be ignored.

## Security

The term 'Security' encompasses two main aspects of the GNTP protocol:

  * Ensuring that only authorized applications can send notifications to the end user's computer
  * Protecting the contents of messages that contain sensitive information

The authorization of messages is accomplished by passing key information that proves that the sending application knows a shared secret with the notification system, namely a password. Users that want to authorize applications must share with them a password that will be used for both authorization and encryption.

**NOTE:** By default, authorization is not required for requests orginating on the local machine.

### Key Generation

When constructing a message that requires authorization or encryption, the first step is to expand the password into a key. The following procedure is used when converting a password to a key:

1. The password is converted an UTF8 byte array
2. A cyptographically secure salt is generated (should be between 4 and 16 bytes)
3. The salt bytes are appended to the password bytes to form the key basis
4. The key is generated by computing the hash of the key basis using one of the supported hashing algorithms
5. The key hash is produced by computing the hash of the key (using the same hashing algorithm used in step 4) and hex-encoding it to a fixed-length string

For developers implementing the GNTP spec, use the Key Generation Check Utility to verify that your key generation routines will generate the correct values.

The supported hashing algorithms are:

  * MD5 (128-bit, 16 byte, 32 character length when hex encoded)
  * SHA1 (160-bit, 20 byte, 40 character length when hex encoded)
  * SHA256 (256-bit, 32 byte, 64 character length when hex encoded)
  * SHA512 (512-bit, 64 byte, 128 character length when hex encoded)

The supported encryption algorithms are:

  * NONE - No encryption; messages are sent in plain text, no IV value should be passed
  * AES - key length: 24 bytes (192 bit), block size: 16 byte (128 bit), iv size: 16 byte (128 bit)
  * DES - key length: 8 bytes (64 bit), block size: 8 byte (64 bit), iv size: 8 byte (64 bit)
  * 3DES - key length: 24 bytes (192 bit), block size: 8 byte (64 bit), iv size: 8 byte (64 bit)

All encryption algorithms should use a block mode of CBC (Cipher Block Chaining) and PKCS5 (PKCS7) padding.

**NOTE:** It is important to keep in mind that some encryption algorithms require keys that are longer than can be generated by some hashing algorithms. As such, not all hash/encryption combinations are valid (ex: MD5 hash produces a 16 byte result, but AES encryption requires a 24-byte key).
There are three different security scenarios when sending messages. For each scenario, the values required in the GNTP information line are listed below:

1. No authorization required (local request):
    * \<encryptionAlgorithmID\> Should be set to NONE
    * \<ivValue\> Should not be included  
    * \<keyHashAlgorithmID\> Should not be included  
    * \<keyHash\> Should not be included  
    * \<salt\> Should not be included

2. Authorization required, but message not encrypted
    * \<encryptionAlgorithmID\> Should be set to NONE  
    * \<ivValue\> Should not be included
    * \<keyHashAlgorithmID\> Set to the type of hash algorithm used in creating the key
    * \<keyHash\> The hex-encoded hash of the key i
    * \<salt\> The hex-encoded value of the salt used when generating the key

3. Encrypted message 
    * \<encryptionAlgorithmID\> Set to the type of encryption algorithm used
    * \<ivValue\> The hex-encoded value of the initialization vector used when encrypting the message
    * \<keyHashAlgorithmID\> Set to the type of hash algorithm used in creating the key
    * \<keyHash\> The hex-encoded hash of the key
    * \<salt\> The hex-encoded value of the salt used when generating the key

The \<salt\> and \<ivValue\> values should be generated uniquely for each individual message.

### Encrypting Messages 

When encrypting GNTP messages, the first line of the message (GNTP information line) MUST NOT be encrypted. The list of messages headers is encrypted as a chunk. Any included binary data sections are encrypted seperately, using the same encryption algorithm and key information as the message headers. When encrypting the binary data sections, the 'Identifier' and 'Length' headers MUST NOT be encrypted (only the actual binary data).

To encrypt the message headers or binary data, use the key generated from the password and salt. Different encryption algorithms require different key lengths and IV sizes, so use the first X bytes of the key as required.

Using the IV sizes specified, create a cryptographically IV value. The hex-encoded value of the IV should be set in the \<ivValue\> portion of the GNTP information line. Using the generated key and IV, encrypt the message headers. The encrypted bytes should be included in the GNTP message after the GNTP information line in place of the plain-text headers.

If additional binary data is also being sent in the message, each binary object should be encrypted using the same key and IV value used to the encrypt the message headers. The value of the 'Length' header should be set to the number of bytes in the encrypted result (not the length of the original unencrypted data). The encrypted binary data bytes should be included in the GNTP message in place of the original unencrypted bytes. See the 'Examples' section below for a sample encrypted request.

    GNTP/1.0 REGISTER AES:696E697469616C697A6174696F6E20766563746F72 SHA1:459EE94A97E68C68A157B018C7F6877D06AE980F.66616B652073616C74
    <encrypted message header bytes>

    Identifier: f082d4e3bdfe15f8f5f2450bff69fb17
    Length: 1207

    <encrypted binary data goes here>

    
If a GNTP request is encrypted, the corresponding GNTP response and callback will also be encrypted. The response and callback will use the same key, salt, key hash algorithm, encryption algorithm, and IV as the request.

**NOTE:** -ERROR responses WILL NOT be encrypted even if the original request was encrypted (because the error could be caused by a malformed message or an unsupported encryption type).

## Ports and Discovery

The standard local GNTP port should be 23053. The port number is not user-configurable.

A GNTP server should advertise over Bonjour, if possible, using the name @"_gntp._tcp.".

## Appendix

### Error-Code values

100 - [reserved]
:   Reserved for future use

200 - TIMED_OUT
:   The server timed out waiting for the request to complete

201 - NETWORK_FAILURE
:   The server was unavailable or the client could not reach the server for any reason

300 - INVALID_REQUEST
:   The request contained an unsupported directive, invalid headers or values, or was otherwise malformed

301 - UNKNOWN_PROTOCOL
:   The request was not a GNTP request

302 - UNKNOWN_PROTOCOL_VERSION
:   The request specified an unknown or unsupported GNTP version

303 - REQUIRED_HEADER_MISSING
:   The request was missing required information

400 - NOT_AUTHORIZED
:   The request supplied a missing or wrong password/key or was otherwise not authorized

401 - UNKNOWN_APPLICATION
:   Application is not registered to send notifications

402 - UNKNOWN_NOTIFICATION
:   Notification type is not registered by the application

500 - INTERNAL_SERVER_ERROR
:   An internal server error occurred while processing the request

### Flash Support

Flash requires that applications making cross-domain network calls must explicitly be allowed to communicate first. For any applications that use Flash as the communication mechanims (websites, AIR, Flex, etc), this requires that the notification system must be able to authorize Flash permission requests. In addition to the GNTP messages defined above, compliant notification daemons should support one additional message type - the Flash policy file request. The request will always look like:

    <policy-file-request/>\0

(where the last byte is a null byte). The correct response is an xml snippet of a valid Flash policy, again followed by a null byte:

    <?xml version="1.0"?> 
    <!DOCTYPE cross-domain-policy SYSTEM "/xml/dtds/cross-domain-policy.dtd">
    <cross-domain-policy>
       <site-control permitted-cross-domain-policies="master-only"/>
       <allow-access-from domain="*" to-ports="*" />
    </cross-domain-policy>\0
        
### Examples

REGISTER, no key required

Request

    GNTP/1.0 REGISTER NONE
    Application-Name: SurfWriter 
    Application-Icon: http://www.site.org/image.jpg 
    X-Creator: Apple Software 
    X-Application-ID: 08d6c05a21512a79a1dfeb9d2a8f262f 
    Notifications-Count: 2 

    Notification-Name: Download Complete 
    Notification-Display-Name: Download completed 
    Notification-Icon: x-growl-resource://cb08ca4a7bb5f9683c19133a84872ca7 
    Notification-Enabled: True 
    X-Language: English 
    X-Timezone: PST 

    Notification-Name: Document Published 
    Notification-Display-Name: Document successfully published 
    Notification-Icon: http://fake.net/image.png 
    Notification-Enabled: False 
    X-Sound: http://fake.net/sound.wav 
    X-Sound-Alt: x-growl-resource://f082d4e3bdfe15f8f5f2450bff69fb17 

    Identifier: cb08ca4a7bb5f9683c19133a84872ca7 
    Length: 4 

    ABCD 

    Identifier: f082d4e3bdfe15f8f5f2450bff69fb17 
    Length: 16

    FGHIJKLMNOPQRSTU

Response

    GNTP/1.0 -OK NONE

