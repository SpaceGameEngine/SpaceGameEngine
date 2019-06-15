# Git 工作流程

本项目的Git工作流程基本上参照[GitHub Flow](https://guides.github.com/introduction/flow/index.html)。

## 具体内容

1. 当你要为项目添加新特性，修复问题，或有什么新的点子时，你都应该创建一个新的分支来储存你的工作。这些分支的名字应由小写单词和下划线组成，如果有对应的Issue，请在最后加入其编号
2. 当你完成你那部分的工作，或者有阶段性的成果，或者想与其他开发者交流想法时，请提交一个拉取请求(Pull Request)，以供代码审查，CI构建和交流讨论。
3. 当特性或修复完成，代码审查通过，CI构建完成后，这个分支将会被合并到master分支后删除。
4. 当新的稳定版本发布时，将会开启新的发布分支，命名如`release-<versio>`。特定于该版本的bug修复将会合并到该分支，并发布新的小版本。

## 参考资料

1. <https://guides.github.com/introduction/flow/index.html>
2. <https://git-scm.com/book/en/v2/Git-Branching-Branching-Workflows>
3. <https://git-scm.com/book/en/v2/Distributed-Git-Distributed-Workflows#ch05-distributed-git>
4. <http://www.ruanyifeng.com/blog/2015/12/git-workflow.html>
