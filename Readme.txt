mission ：是基于大疆osdk3.9开发的m600飞机端的飞控项目，实现了一键起飞，一键降落，一键返航，航点规划，航点拍照功能；
	使用教程：去大疆官网下载，下载osdk3.9。将osdk按照大疆的文档进行编译 ，将osdk中的mission文件夹替换成本文件夹即可（中间修改了cmake所以有些依赖需要安装）
        cd Onboard-SDK
        mkdir build
        cd build
        cmake ..
        make djiosdk-core
        sudo make install djiosdk-core


onboard4.3.google：基于qt5.9开发的M600无人机地面站：使用的是qt5.9+msvc2015-64bit进行开发的，在安装qt5.9时要求先安装vs2015
