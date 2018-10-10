index 기능
==========================

세 가지의 index 기능이 추가되었습니다. 

### 추가된 실행 옵션

-addressindex : 지갑 주소에 대한 balance, txids, unspent output 을 조회할 수 있습니다.

-timestampindex : 시간 범위 내의 블럭 해시를 조회할 수 있습니다.

-spentindex : 지불된 거래의 txid 및 입력과 출력을 조회할 수 있습니다.

### 추가된 rpc call

index 기능 추가에 따른 rpc call은 아래와 같습니다. 각 기능에 대한 사용법은 

```bash
hdac-cli hdac help <rpc call> 
```
으로 확인할 수 있습니다.

-----------------------------------------------
* getaddressmempool : 지갑 주소에 대해 mempool 내의 잔액을 조회합니다.
* getaddressutxos : 지갑 주소에 대해 utxo를 조회합니다.
* getaddressdeltas : 지갑 주소에 대해 잔액의 변동을 조회합니다.
* getaddresstxids : 지갑 주소에 대해 txid를 조회합니다.
* getaddressbalance : 지갑 주소에 대해 현재 잔액을 조회합니다.
* getblockhashes : 설정한 시간 범위 내의 블럭 해시를 조회할 수 있습니다.
* getspentinfo : txid로 지정한 거래에서 송금한 출력이 이후에 어떤 거래에서 사용되었는지를 조회할 수 있습니다.
* getblockdeltas : 블록 내의 잔액 변동을 조회합니다.

### rpc 명령 사용 방법
#### getaddressmempool
```bash
Returns all mempool deltas for an address (requires addressindex to be enabled).

Arguments:
{
  "addresses"
    [
      "address"  (string) The base58check encoded address
      ,...
    ]
}

Result:
[
  {
    "address"  (string) The base58check encoded address
    "txid"  (string) The related txid
    "index"  (number) The related input or output index
    "qty"  (number) The difference of dac
    "timestamp"  (number) The time the transaction entered the mempool (seconds)
    "prevtxid"  (string) The previous txid (if spending)
    "prevout"  (string) The previous transaction output index (if spending)
  }
]

Example : hdac-cli hdac getaddressmempool '{"addresses": ["HDFd45jQ1e6khUwojLd8ndFWa6QJd5ntVz"]}'
```

#### getaddressutxos
```bash
Returns all unspent outputs for an address (requires addressindex to be enabled).

Arguments:
{
  "addresses"
    [
      "address"  (string) The base58check encoded address
      ,...
    ],
  "chainInfo"  (boolean) Include chain info with results
}

Result
[
  {
    "address"  (string) The address base58check encoded
    "txid"  (string) The output txid
    "height"  (number) The block height
    "outputIndex"  (number) The output index
    "script"  (strin) The script hex encoded
    "qty"  (number) The number of dac of the output
  }
]

Example : hdac-cli hdac getaddressutxos '{"addresses": ["HDFd45jQ1e6khUwojLd8ndFWa6QJd5ntVz"]}'
```
#### getaddressdeltas

```bash
Returns all changes for an address (requires addressindex to be enabled).

Arguments:
{
  "addresses"
    [
      "address"  (string) The base58check encoded address
      ,...
    ]
  "start" (number) The start block height
  "end" (number) The end block height
  "chainInfo" (boolean) Include chain info in results, only applies if start and end specified
}

Result:
[
  {
    "qty"  (number) The difference of dac
    "txid"  (string) The related txid
    "index"  (number) The related input or output index
    "height"  (number) The block height
    "address"  (string) The base58check encoded address
  }
]

Example : hdac-cli hdac getaddressdeltas '{"addresses": ["HDFd45jQ1e6khUwojLd8ndFWa6QJd5ntVz"]}'
```
#### getaddresstxids

```bash
Returns the txids for an address(es) (requires addressindex to be enabled).

Arguments:
{
  "addresses"
    [
      "address"  (string) The base58check encoded address
      ,...
    ]
  "start" (number) The start block height
  "end" (number) The end block height
}

Result:
[
  "transactionid"  (string) The transaction id
  ,...
]

Example: hdac-cli hdac getaddresstxids '{"addresses": ["HDFd45jQ1e6khUwojLd8ndFWa6QJd5ntVz"]}'
```
#### getaddressbalance

```bash
Returns the balance for an address(es) (requires addressindex to be enabled).

Arguments:
{
  "addresses"
    [
      "address"  (string) The base58check encoded address
      ,...
    ]
}

Result:
{
  "qty"  (string) The current balance in dac
  "received"  (string) The total number of satoshis received (including change)
}

Example: hdac-cli hdac getaddressbalance '{"addresses": ["HDFd45jQ1e6khUwojLd8ndFWa6QJd5ntVz"]}'
```
#### getblockhashes

```bash
Returns array of hashes of blocks within the timestamp range provided.

Arguments:
1. high         (numeric, required) The newer block timestamp
2. low          (numeric, required) The older block timestamp
3. options      (string, required) A json object
    {
      "noOrphans":true   (boolean) will only include blocks on the main chain
      "logicalTimes":true   (boolean) will include logical timestamps with hashes
    }

Result:
[
  "hash"         (string) The block hash
]
[
  {
    "blockhash": (string) The block hash
    "logicalts": (numeric) The logical timestamp
  }
]

Examples: hdac-cli hdac getblockhashes 1231614698 1231024505

```
#### getspentinfo

```bash
Returns the txid and index where an output is spent.

Arguments:
{
  "txid" (string) The hex string of the txid
  "index" (number) The start block height
}

Result:
{
  "txid"  (string) The transaction id
  "index"  (number) The spending input index
  ,...
}

Example: hdac-cli hdac getspentinfo '{"txid": "0437cd7f8525ceed2324359c2d0ba26006d92d856a9c20fa0241106ee5a597c9", "index": 0}'
```
#### getblockdeltas

```bash
Returns all changes for an block.

Arguments:
1. "hash"  (string) The block hash

Result:
[
  {
    "hash"  (string) The block hash
    "confirmations"  (number) The number of comfirmations
    "size"  (number) The size of block
    "height"  (number) The block height
    "version"  (number) The block chain version
    "merkleroot"  (string) The block merkle tree
    "deltas"  (string) The transactions in block
    "time"  (number) The unix time
    "mediantime"  (number) The mediantime
    "nonce"  (string) The block nonce value
    "bits"  (string) The block bits
    "difficulty"  (string) The block difficulty
    "chainwork"  (string) The block chainwork
    "previousblockhash"  (string) The previous block hash
    "nextblockhash"  (string) The next block hash
  }
]

Examples:
> hdac-cli hdac getblockdeltas "ee7af6dd4969a2c3c5ae525d29a926212e240bde26c4f54de16cf8bfeedb6a2d"
```
