WPA_SUPPLICANT DEMO
====================

This project is a demo for roaming in a hotspot 2.0 capable environment. I implement this demo on a PC in two parts, including auto roaming in wpa_supplicant and a user client with Qt4. The client will retrieve all ANQP information from access points, and do an algorithm like filters to select the most suitable AP to associate. After Disconnect event occurs, the client will automatically use ANQP information to roam to another AP without loss of connection.

License: [BSD](http://opensource.org/licenses/bsd-3-clause) and [GPL2](http://www.gnu.org/licenses/gpl-2.0.html)

Author: [Further Lee](http://github.com/furtherLee)
