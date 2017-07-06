#include <boost/program_options.hpp>

using namespace boost::program_options;
using namespace std;

//g++ -g -Wall parse_command.cpp -lboost_program_options

int main(int argc, char* argv[])
{
	options_description opts("demo options");

	//TODO 需要使用try-catch来处理未定义的选项名
	//增加两个程序选项
	opts.add_options()
		("help", "just a help info")
		//value<string>()是program_options库的一个辅助
		//模板函数，它产生一个typed_value对象，用于保存选项值
		("filename", value<string>(), "to find a file")
		;

	//选项存储map容器
	variables_map vm;

	//解析存储
	//parse_command_line()函数执行命令行参数的解析
	//store()把解析的结果存储到variables_map的实例vm中。
	store(parse_command_line(argc, argv, opts), vm);

	//解析完成，实现选项处理逻辑
	//成员函数count()测试选项的数量
	if (vm.count("help")) 	//处理帮助选项
	{
		cout << opts << endl;
		return 0;
	}
	if (vm.count("filename")) //要查找的文件名
	{
		cout << "find" << vm["filename"].as<string>() << endl;
	}
	if (vm.size() == 0)
	{
		cout << "no options" << endl;
	}
}
