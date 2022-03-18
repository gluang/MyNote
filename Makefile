all: help

BOOK_NAME="notebook"
COVER_PATH="assets/cover.png"
CURRENT_DATE=$(shell date +"%Y-%m-%d")
BOOK_NAME_DATE="${BOOK_NAME}-${CURRENT_DATE}"
CONTAINER_NAME=${BOOK_NAME}

## container: 构建容器
.PHONY: container
container:
	@docker run -itd \
		-v ${PWD}/README.md:/srv/gitbook/README.md \
		-v ${PWD}/SUMMARY.md:/srv/gitbook/SUMMARY.md \
		-v ${PWD}/book.json:/srv/gitbook/book.json \
		-v ${PWD}/.bookignore:/srv/gitbook/.bookignore \
		-v ${PWD}/设计模式:/srv/gitbook/设计模式 \
		-v ${PWD}/数据结构:/srv/gitbook/数据结构 \
		-v ${PWD}/算法:/srv/gitbook/算法 \
		-v ${PWD}/C++:/srv/gitbook/C++ \
		-v ${PWD}/assets:/srv/gitbook/assets \
		-v ${PWD}/docs:/srv/gitbook/_book \
		-p 4000:4000 \
		--name ${CONTAINER_NAME} \
		gluang/gitbook:latest bash

## serve: 启动本地 web 服务，监听 4000 端口
.PHONY: serve
serve:
	@/bin/bash script.sh print_ip_address ${CONTAINER_NAME} \
		&& docker exec -it ${CONTAINER_NAME} /usr/local/bin/gitbook serve \
		|| echo "\nstop!"

## cover: 添加封面图
.PHONY: cover
cover:
	@docker exec -it ${CONTAINER_NAME} convert -resize 1800x2360! ${COVER_PATH} cover.jpg

## assets: 生成 pdf、epub 和 mobi
assets: pdf epub mobi

## pdf: 生成 pdf
.PHONY: pdf
pdf: cover
	@mkdir -p assets
	@docker exec -it ${CONTAINER_NAME} /usr/local/bin/gitbook pdf . assets/${BOOK_NAME_DATE}.pdf

## epub: 生成 epub
.PHONY: epub
epub: cover
	@mkdir -p assets
	@docker exec -it ${CONTAINER_NAME} /usr/local/bin/gitbook epub . assets/${BOOK_NAME_DATE}.epub

## mobi: 生成 mobi
.PHONY: mobi
mobi: cover
	@mkdir -p assets
	@docker exec -it ${CONTAINER_NAME} /usr/local/bin/gitbook mobi . assets/${BOOK_NAME_DATE}.mobi

## rm-container: 删除容器
.PHONY: rm-container
rm-container:
	@docker stop ${CONTAINER_NAME} \
		&& docker rm ${CONTAINER_NAME} 

## gen-toc: 生成目录
.PHONY: gen-toc
gen-toc:
	@/bin/bash script.sh generate_toc 设计模式 数据结构 C++ 算法

## help: 打印命令帮助信息
.PHONY: help
help:
	@echo "Usage:"
	@sed -n 's/^##//p' ${MAKEFILE_LIST} | column -t -s ':' |  sed -e 's/^/ /'
