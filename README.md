# GinShio STL

## 项目背景
此项目主要是个人用于学习 `STL` 和 `模板`, 以此巩固 C++ 学习中薄弱的知识点与编码经验, 并学习 C++ 、 git 以及周边工具链, 并完成一个基于 **C++11** 的遵循标准的STL。

另一方面, 由于整个STL庞大, 由 *容器* 、 *迭代器* 、 *分配器* 等多个组件组成, 此项目将实现一个组件可拆分、灵活组合的STL。


## 安装
这个, emmm...., 整个库还没有完成, 并且还不会做动态库和静态库, 先源码安装吧, 待进一步学习后再来完成打包等工作。


## 使用
与标准库 STL 使用方法相同, 头文件为 **include/** 下的 `.hpp` 文件, 仅需要包含头文件并替换名称空间即可, 以下以 *vector* 为示例做简单的用法展示。

```cpp
// #include "inlcude/vector.hpp"
ginshio::stl::vector<int> v{1, 2, 3, 4, 5};
v.push_back(6);  // {1, 2, 3, 4, 5, 6}
v.erase(v.begin() + 2, v.end());  // {1, 2}
```


## 相关项目
 - [libstdc++](https://gcc.gnu.org/onlinedocs/libstdc++/)
 - [libc++](https://libcxx.llvm.org/docs/index.html)
 - [STLport](http://www.stlport.org/)
 - [Qt Container](https://doc.qt.io/qt-5/containers.html)
 - [EASTL](https://github.com/electronicarts/EASTL)


## TODO
  - 容器 及 适配器
    - 顺序容器 (string)
    - 容器适配器 (priority_queue)
    - 有序关联容器的 `note_type` 与 `find` 相关函数, 以及其他底层实现 (**avl tree**)
    - 无序关联容器
  - 分配器
  - 迭代器
  - 算法
  - 完善的 **TestCase** 、 **Benchmark** 和 **Document**
  - 禁止异常
  - 修改为 pmr(polymorphic) 分配器


## 参与贡献方式
你可以开启 **issue** 或提交 **PR** 等方式, 感谢你对此项目的关注与贡献


## 开源协议
[BSD](LICENSE) © Xin 'GinShio' Liu
