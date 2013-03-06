VPATH = ../Src/

Object = ./SysCtrlNode.o ./UpdatePlNodeState.o ./RequestPlNode.o ./RecordConnInfo.o ./ProcessReqFromClient.o

SysCtrlNode: $(Object)
	gcc -o SysCtrlNode -lpthread $(Object)

./SysCtrlNode.o: ../Src/SysCtrlNode.h ../Src/ProcessReqFromClient.h ../Src/UpdatePlNodeState.h
./UpdatePlNodeState.o: ../Src/UpdatePlNodeState.h ../Src/RequestPlNode.h ../Src/RecordConnInfo.h
./RequestPlNode.o: ../Src/RequestPlNode.h
./RecordConnInfo.o: ../Src/RecordConnInfo.h ../Src/UpdatePlNodeState.h
./ProcessReqFromClient.o: ../Src/ProcessReqFromClient.h ../Src/RecordConnInfo.h ../Src/RequestPlNode.h

clean:
	rm $(Object) SysCtrlNode

