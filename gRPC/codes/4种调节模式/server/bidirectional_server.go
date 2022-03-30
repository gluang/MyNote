package main

import (
	"flag"
	"google.golang.org/grpc"
	pb "grpc-demo/proto/bidirectional"
	"io"
	"log"
	"net"
)

type GreeterServer struct{}

func (s *GreeterServer) SayRoute(stream pb.Greeter_SayRouteServer) error {
	n := 0
	for {
		_ = stream.Send(&pb.HelloReply{Message: "say.route"})
		
		resp, err := stream.Recv()
		if err == io.EOF {
			return nil
		}
		if err != nil {
			return err
		}

		n++
		log.Printf("resp: %v", resp)
	}
}

var port string

func init() {
	flag.StringVar(&port, "p", "8000", "启动端口号")
	flag.Parse()
}

func main() {
	// 创建 gRPC Server 对象，可以理解为它是 Server 端的抽象对象
	server := grpc.NewServer()
	// 将 GreeterServer（其包含需要被调用的服务端接口）注册到 gRPC Server 的内部注册中心
	// 这样可以在接受到请求时，通过内部的 “服务发现”，发现该服务端接口并转接进行逻辑处理
	pb.RegisterGreeterServer(server, &GreeterServer{})
	// 创建 Listen，监听 TCP 端口
	lis, _ := net.Listen("tcp", ":"+port)
	// gRPC Server 开始 lis.Accept，直到 Stop 或 GracefulStop
	server.Serve(lis)
}
