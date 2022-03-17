#!/bin/bash

# 在 SUMMARY.md 中生成目录（可以多建一层目录）
generate_toc() {
	echo "# Summary" >SUMMARY.md
	echo >>SUMMARY.md
	echo "* [简介](README.md)" >>SUMMARY.md

	# 存放内容文件的文件夹在2个或以上，则在SUMMARY.md中生成子标题
	if [ $# -ne 2 ]; then
		gen_sub_title="true"
	fi

	# 因为从Makefile中传入的第一个参数是 generate_toc，所以从第二个参数开始遍历
	until [ $# -eq 1 ]; do

		if [ "$gen_sub_title" == "true" ]; then
			echo >>SUMMARY.md
			echo "### $2" >>SUMMARY.md
		fi

		save_sup_category=""

		for file in $(find $2 -name "*.md" | sort); do
			# 源文件绝对路径：/home/gluang/temp/contents/one/chapter01.md
			# 或 /home/gluang/temp/contents/chapter01.md
			# file：contents/one/chapter01.md 或 contents/chapter01.md

			# chapter01.md
			file_name_include_suffix="${file##*/}"
			# 如果不是 README.md 以及不是 参考资料.md 则继续（README.md作为该章节的总结，不加入）
			if [ "$file_name_include_suffix" != "references.md" ]; then
				# chapter01
				file_name_exclude_suffix="${file_name_include_suffix%.md}"
				# contents/one 或 contents
				file_absolute_path="${file%/$file_name_include_suffix}"
				# /one 或 空
				new_file_absolute_path="${file_absolute_path#$2}"

				# 1. 如果不为空，则为 /one，去 / 变成：one
				if [ -n "$new_file_absolute_path" ]; then
					new_file_absolute_path="${new_file_absolute_path#/}"
					# 跟其不相等，也即有小标题，
					if [ "$new_file_absolute_path" != "$save_sup_category" ]; then
						# 如果该章节的 README.md 文件不存在，则创建该文件
						readme_file="$file_absolute_path/README.md"
						if [ ! -e $readme_file ]; then
							title="<h1 align='center'>$new_file_absolute_path</h1>"
							echo $title >$readme_file
						fi

						echo "* [$new_file_absolute_path]($readme_file)" >>SUMMARY.md
						save_sup_category=$new_file_absolute_path
					fi

					echo "    * [$file_name_exclude_suffix]($file)" >>SUMMARY.md
				# 2. 如果为空，则
				else
					echo "* [$file_name_exclude_suffix]($file)" >>SUMMARY.md
				fi
			fi
			# 如果刚好又是
			# if [ "$file_name_include_suffix" == "README.md" ]
		done

		# # 结尾添加 参考资料.md
		# references="contents/references.md"
		# if [ ! -e $references ]
		# then
		# 	title="<h1>参考资料</h1>"
		# 	echo $title > $references
		# fi
		# echo "* [参考资料]($references)" >>SUMMARY.md
		shift
	done
}

print_ip_address() {
	# # $2 是端口号，$3 是容器名
	# if [ -n "$2" ]; then
	# 	id=`docker inspect $3 | grep '"IPAddress":' | awk '{print $2}'|sed 's/[",]//g'|sed -n '1p'`
	# 	echo "：打开: $id:$2"
	# else
	# 	echo "未加端口号"
	# fi
	id=$(docker inspect $2 | grep '"IPAddress":' | awk '{print $2}' | sed 's/[",]//g' | sed -n '1p')
	echo "打开: http://$id:4000"
}

case "$1" in
# 需要用 $@ 将参数传递进去
generate_toc) generate_toc $@ ;;
print_ip_address) print_ip_address $@ ;;
*) echo "未找到相应的指令" ;;
esac
