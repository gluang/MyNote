.PHONY: init help

EXC_NAME=grpc-demo

## init: 初始化项目
init:
	@go mod init "${EXC_NAME}"

help:
	@echo "Usage:"
	@sed -n 's/^##//p' ${MAKEFILE_LIST} | column -t -s ':' |  sed -e 's/^/ /'

## proto: 编译生成 proto
.PHONY: proto
proto:
	@protoc --go_out=plugins=grpc:. ./proto/*.proto

## get: 拉取第三方库
.PHONY: get
get: grpc protobuf

grpc:
	@go get -u google.golang.org/grpc@v1.29.1
protobuf:
	@go get github.com/golang/protobuf/proto@v1.4.2

## clean: 清理中间目标文件
clean:
	@rm -f cmd/*
	@rm -rf proto/*/*.pb.go


## unary: 一元RPC的编译
.PHONY: unary
unary:
	@protoc --go_out=plugins=grpc:. ./proto/unary/unary.proto
	@go build -o cmd/unary_client client/unary_client.go
	@go build -o cmd/unary_server server/unary_server.go

## serverside: 服务端流式RPC的编译
.PHONY: serverside
serverside:
	@protoc --go_out=plugins=grpc:. ./proto/serverside/serverside.proto
	@go build -o cmd/serverside_client client/serverside_client.go
	@go build -o cmd/serverside_server server/serverside_server.go

## clientside: 客户端流式RPC的编译
.PHONY: clientside
clientside:
	@protoc --go_out=plugins=grpc:. ./proto/clientside/clientside.proto
	@go build -o cmd/clientside_client client/clientside_client.go
	@go build -o cmd/clientside_server server/clientside_server.go

## bidirectional: 双向流式RPC的编译
.PHONY: bidirectional
bidirectional:
	@protoc --go_out=plugins=grpc:. ./proto/bidirectional/bidirectional.proto
	@go build -o cmd/bidirectional_client client/bidirectional_client.go
	@go build -o cmd/bidirectional_server server/bidirectional_server.go
