import os
import shutil

def delete_dirs_and_files(dir_path, search_strs):
    for search_str in search_strs:
        for root, dirs, files in os.walk(dir_path, topdown=False):
            for name in dirs:
                if search_str.lower() in name.lower():
                    full_path = os.path.join(root, name)
                    shutil.rmtree(full_path)
                    print(f'Deleted folder: {full_path}')

            for name in files:
                if search_str.lower() in name.lower():
                    full_path = os.path.join(root, name)
                    os.remove(full_path)
                    print(f'Deleted file: {full_path}')

# 设置为当前文件所在的目录
directory_path = os.path.dirname(os.path.realpath(__file__))

# 替换为你的搜索字符串列表
search_strings = ['test', 'fuzz', 'mock', 'benchmark', 'example']

delete_dirs_and_files(directory_path, search_strings)