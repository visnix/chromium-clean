import os
import sys

def find_files(directory, extension):
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith(extension):
                print(os.path.relpath(os.path.join(root, file), directory))

# 获取当前工作目录
directory = os.getcwd()

# 获取命令行参数
if len(sys.argv) > 1:
    extension = '.' + sys.argv[1]
else:
    print("Please provide a file extension.")
    sys.exit(1)

find_files(directory, extension)