import os
import shutil

def delete_test_dirs_and_files(dir_path):
    for root, dirs, files in os.walk(dir_path, topdown=False):
        for name in dirs:
            if 'test' in name:
                full_path = os.path.join(root, name)
                shutil.rmtree(full_path)
                print(f'Deleted folder: {full_path}')

        for name in files:
            if 'test' in name:
                full_path = os.path.join(root, name)
                os.remove(full_path)
                print(f'Deleted file: {full_path}')

# 请替换为你想要处理的目录
directory_path = '/Users/m1/github/chromium-main'
delete_test_dirs_and_files(directory_path)