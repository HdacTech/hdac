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
getaddressmempool : 지갑 주소에 대해 mempool 내의 잔액을 조회합니다.

getaddressutxos : 지갑 주소에 대해 utxo를 조회합니다.

getaddressdeltas : 지갑 주소에 대해 잔액의 변동을 조회합니다.

getaddresstxids : 지갑 주소에 대해 txid를 조회합니다.

getaddressbalance : 지갑 주소에 대해 현재 잔액을 조회합니다.

getblockhashes : 설정한 시간 범위 내의 블럭 해시를 조회할 수 있습니다.

getspentinfo : txid로 지정한 거래에서 송금한 출력이 이후에 어떤 거래에서 사용되었는지를 조회할 수 있습니다.

getblockdeltas : 블록 내의 잔액 변동을 조회합니다.
