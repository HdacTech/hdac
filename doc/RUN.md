HADC Core
=============

Setup
---------------------
Hdac Core is the original Hdac client and it builds the backbone of the network. It downloads and, by default, stores the entire history of Hdac transactions ; depending on the speed of your computer and network connection, the synchronization process can take anywhere from a few hours to a day or more.

To download Hdac Core, visit [https://hdactech.com](https://hdactech.com).

Running
---------------------
The following are some helpful notes on how to run Hdac on your native platform.

### Linux

Unpack the files into a directory and run:

- hdacd hdac@seednode-URL:port
- port is `8823`, seed node are `seed.as.hdactech.com`, `seed.us.hdactech.com`,`seed.eu.hdactech.com`
- default data directory is `$HOME/.hdac/hdac`
- if you want to change HDAC data directory, user runtime parameter "-datadir".

- running example
```
$ hdacd hdac@seed.as.hdactech.com:8823

Hdac 1.01.0 Daemon (latest protocol 20000)

Retrieving blockchain parameters from the seed node seed.as.hdactech.com:8823 ...
Other nodes can connect to this node using:
hdacd hdac@xxx.xxx.xxx.xxx:8823

This host has multiple IP addresses, so from some networks:
hdacd hdac@xxx.xxx.xxx.xxx:8823

Listening for API requests on port 8822 (local only - see rpcallowip setting)

Node ready.
```

- daemon runtime parameters ( daemon, datadir, etc... )
```
$ hdacd hdac@seed.as.hdactech.com:8823 -daemon -datadir="$HOME/datadir"
```

### Windows

Unpack the files into a directory, and then run hdacd.exe.

- default data directory is `%APPDATA%/Hdac/hdac`
- if you want to change data directory, user runtime parameter "-datadir".
- how to connection is the same as on linux.


### Mac OS

Unpack the files into a directory and run:

- default data directory is `$HOME/.hdac/hdac`
- if you want to change HDAC data directory, user runtime parameter "-datadir".
- how to connection is the same as on linux.

### Common Operations

- Run HDAC with interactive mode (※ not supported in Windows).
- to issue commands without typing "hdac-cli hdac ..." every time, use interactive mode
```
$ hdac-cli hdac

Hdac 1.01.0 RPC client


Interactive mode

hdac:
```
---------------------------------------------------------------------

- get general node info : `hdac-cli hdac getinfo`
```
$ hdac-cli hdac getinfo
{"method":"getinfo","params":[],"id":1,"chain_name":"hdac"}

{
    "version" : "1.01.0",
    "protocolversion" : 20000,
    "chainname" : "hdac",
    "protocol" : "hdac",
    "port" : 8823,
    "nodeaddress" : "hdac@xxx.xxx.xxx.xxx:8823",
    "burnaddress" : "HJXXXXXXXXXXXXXXXXXXXXXXXXXXVarS5i",
    "balance" : 0.00000000,
    "reindex" : false,
    "blocks" : xxxxx,
    "chain-blocks" : xxxxx,
    "timeoffset" : 32,
    "connections" : 8,
    "proxy" : "",
    "difficulty" : xx.xxxx,
    "testnet" : false,
    "keypoololdest" : 1522903582,
    "keypoolsize" : 2,
    "paytxfee" : 0.00000000,
    "relayfee" : 0.01000000,
    "errors" : ""
}

```
---------------------------------------------------------------------

- get default address of my wallet : `hdac-cli hdac getaddresses`

```
$ hdac-cli hdac getaddresses
{"method":"getaddresses","params":[],"id":1,"chain_name":"hdac"}

[
    "HAr1viBPFik7X7UHZonVqmPnryiCgDijij"
]
```
---------------------------------------------------------------------

- get new address for my wallet : `hdac-cli hdac getnewaddress`

```
$ hdac-cli hdac getnewaddress
{"method":"getnewaddress","params":[],"id":1,"chain_name":"hdac"}

H8dkwEgbF1aayGd3WfgfENWkQoQeSeghtm
```
---------------------------------------------------------------------

- get ballance of my wallet : `hdac-cli hdac getbalance`

```
$ hdac-cli hdac getbalance
{"method":"getbalance","params":[],"id":1,"chain_name":"hdac"}

99.00000000
```
---------------------------------------------------------------------

- send money : `hdac-cli hdac send "address" "amount"`

```
$ hdac-cli hdac send HAr1viBPFik7X7UHZonVqmPnryiCgDijij 100
{"method":"send","params":["HAr1viBPFik7X7UHZonVqmPnryiCgDijij",100],"id":1,"chain_name":"hdac"}

8d40da1a8160427dc72f51cf18408505b0e99e369d86970b1a375a11a621d62a
```
---------------------------------------------------------------------

- For other commands, please run help. : `hdac-cli hdac help`

```
$ hdac-cli hdac help
{"method":"help","params":[],"id":1,"chain_name":"hdac"}

== Blockchain ==
getbestblockhash
getblock "hash"|height ( verbose )
getblockchaininfo
getblockcount
getblockhash index
getchaintips
getdifficulty
getmempoolinfo
getrawmempool ( verbose )
gettxout "txid" n ( includemempool )
gettxoutsetinfo
listassets ( asset-identifier(s) verbose count start )

....


```
---------------------------------------------------------------------



Building
---------------------
The Hdac repo's [root README](/README.md) contains relevant information on the Building.
