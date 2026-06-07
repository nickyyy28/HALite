import fdt

def traverse_node(node, depth=0):
    """
    递归遍历设备树节点及其属性
    :param node: 当前的 fdt.Node 对象
    :param depth: 当前节点深度（用于打印层级缩进）
    """
    # 根据深度生成缩进，使得输出具备直观的层级感
    indent = "    " * depth
    print(f"{indent}{node.name} {{")

    # 1. 遍历并输出当前节点的所有属性
    for prop in node.props:
        # fdt 库中的 Property 对象重写了 __str__ 方法
        # str(prop) 能直接将其格式化为设备树源码(.dts)样式，例如：compatible = "arm,cortex-a53"
        print(f"{indent}    {prop}")

    # 2. 递归遍历当前节点的所有子节点
    for child in node.nodes:
        traverse_node(child, depth + 1)

    print(f"{indent} }};")

def parse_and_traverse_dtb(file_path):
    """
    读取并解析 DTB 文件，从根节点启动遍历
    """
    try:
        # 必须以二进制读取模式 (rb) 打开 dtb 文件
        with open(file_path, 'rb') as f:
            dtb_data = f.read()

        # 使用 parse_dtb 解析二进制流，返回一个 Fdt 对象
        dt = fdt.parse_dtb(dtb_data)

        # 获取设备树根节点 '/'
        root_node = dt.get_node('/')

        if root_node:
            print(f"--- 开始解析并遍历 DTB: {file_path} ---")
            traverse_node(root_node)
        else:
            print("错误：未找到根节点 '/'。")

    except FileNotFoundError:
        print(f"错误：找不到文件 '{file_path}'。请检查路径。")
    except Exception as e:
        print(f"解析过程中发生异常：{e}")

if __name__ == '__main__':
    DTB_FILE_PATH = '../../cmake-build-debug/stm32h743xi.dtb'
    parse_and_traverse_dtb(DTB_FILE_PATH)