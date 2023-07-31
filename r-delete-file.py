# 递归删除所给目录及其子目录下所有不在编程语言列表中的文件
import os

# 编程语言及其对应的文件后缀
extensions = {
    'C++': ['.cpp', '.cc', '.cxx', '.c++', '.h', '.hpp'],
    'C/C++ Header': ['.h', '.hh', '.hpp', '.hxx'],
    'Java': ['.java', '.class', '.jar'],
    'JavaScript': ['.js', '.jsx'],
    'Rust': ['.rs'],
    'Python': ['.py', '.pyc', '.pyo', '.pyd', '.pyw'],
    'Objective-C++': ['.mm'],
    'C': ['.c'],
    'HTML': ['.html', '.htm'],
    'TypeScript': ['.ts', '.tsx'],
    'Assembly': ['.asm', '.s'],
    'Markdown': ['.md'],
    'IDL': ['.idl'],
    'diff': ['.diff', '.patch'],
    'Mojo': ['.mojom'],
    'Objective-C': ['.m'],
    'Bazel': ['.bazel', '.bzl'],
    'C#': ['.cs'],
    'Protocol Buffers': ['.proto'],
    'CSS': ['.css'],
    'YAML': ['.yaml', '.yml'],
    'JSON5': ['.json5'],
    'Windows Module Definition': ['.def'],
    'Bourne Shell': ['.sh'],
    'reStructuredText': ['.rst'],
    'Perl': ['.pl', '.pm'],
    'CMake': ['.cmake', '.ctest'],
    'm4': ['.m4'],
    'PHP': ['.php', '.php3', '.php4', '.php5', '.phtml'],
    'make': ['.mk', '.make'],
    'Starlark': ['.bzl'],
    'Swift': ['.swift'],
    'TOML': ['.toml'],
    'DTD': ['.dtd'],
    'Jinja Template': ['.jinja', '.j2'],
    'Bourne Again Shell': ['.bash'],
    'Vuejs Component': ['.vue'],
    'Maven': ['.pom', '.mvn'],
    'Groovy': ['.groovy', '.gradle'],
    'Pascal': ['.pas', '.p'],
    'TeX': ['.tex', '.sty', '.cls', '.dtx', '.ins', '.ltx'],
    'SQL': ['.sql'],
    'Ruby': ['.rb'],
    'Windows Resource File': ['.rc'],
    'DOS Batch': ['.bat', '.cmd'],
    'WiX source': ['.wixproj'],
    'Meson': ['.meson', '.wrap'],
    'MSBuild script': ['.csproj', '.vbproj', '.vcxproj', '.proj'],
    'Jupyter Notebook': ['.ipynb'],
    'Lisp': ['.lisp', '.lsp', '.cl', '.fasl'],
    'Kotlin': ['.kt', '.kts'],
    'yacc': ['.y', '.yy'],
    'XSD': ['.xsd'],
    'Go': ['.go'],
    'vim script': ['.vim'],
    'awk': ['.awk'],
    'Gradle': ['.gradle'],
    'INI': ['.ini'],
    'CoffeeScript': ['.coffee'],
    'ANTLR Grammar': ['.g4'],
    'Dart': ['.dart'],
    'PowerShell': ['.ps1', '.psm1', '.psd1'],
}

# 打平 extensions 字典，将所有文件扩展名放入一个集合中
allowed_extensions = {ext for exts in extensions.values() for ext in exts}

def delete_unwanted_files(dir_path):
    for root, dirs, files in os.walk(dir_path, topdown=False):
        for name in files:
            _, ext = os.path.splitext(name)
            if ext not in allowed_extensions:
                full_path = os.path.join(root, name)
                os.remove(full_path)
                print(f'Deleted file: {full_path}')

# 请替换为你想要处理的目录
directory_path = '/Users/m1/github/chromium-main'
delete_unwanted_files(directory_path)