WPA_SUPPLICANT DEMO
====================

This project is a demo for roaming in a hotspot 2.0 capable enviornment. We will implement this demo on a PC in two parts, including auto roaming in `wpa_supplicant` and a user client of `Gnome Network Manager`. This is the first part.

Workflow
--------

The algorithm engine is called an *Arbiter*. The arbiter will decide which AP to associate when the current connection is down. Arbiter register an DISCONNECTED event. When such an event happens, it will use ANQP Roaming Consortium List to ask whoever has interworking capability. After a defined time, it collect information retrieved by `wpa_supplicant`, and then start the selection process.

The selection process is a kind of filtering. There is a list of filters, which may be defined in some configuration or hard coded (current way). When the hotspot 2.0 information is got from wpa_supplicant, firstly, arbiter put them into the first filter. Then the filter remove some AP candidates according to some rules. Next, it passes the remained candidates to the second filter, etc.

Currently, there are only two kinds of filters, including a Consortium filter and Random filter. Consortium filter select those AP who is in the same consortium of the last connected AP. A Random filter is trying to select only one AP from candidates. Further filter may be Fee filter or something like to filter signal strength or authentication methods. In one word, this is an algorithm framework.

Cooperation with Gnome Network Manager
--------------------------------------

Gnome Network Manager is now a client for showing how `Arbiter` select an AP using hotspot 2.0. It will show up a window to record every step in the algorithm. The communication between `wpa_supplicant` and `Network Manager` is via `dbus`. More details are in the project of the second part.

License: [BSD](http://opensource.org/licenses/bsd-3-clause) and [GPL2](http://www.gnu.org/licenses/gpl-2.0.html)
Author: [Further Lee](http://github.com/furtherLee)

