#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <map>
using namespace std;

#define MAX 2^64-1 // 2^64-1 = 18446744073709551615
#define MAXN 100 //二维数组下标最大值

struct WORD{ //单词结构体
    string name; //单词
    string type; //类型
    int line_number; //行号
};
struct ERROR{ //错误结构体
    char type;  //错误类型
    int line_number; //行号
};
struct SYMBOL{ //符号表结构体
    string name; //单词
    string type; //类型
    string value; //值
    string address; //地址
    string arr[MAXN][MAXN]; //二维数组
    int dim1; //一维数组长度
    int dim2; //二维数组长度
    int dim; //数组维数
    vector<int> func_params; //函数参数
};
class TABLE{ //符号表类
public:
    TABLE() {
        parent=NULL;
        symbol.clear();
    }
public:
    map<string, SYMBOL> symbol; //符号表
    struct TABLE *parent; //父符号表
}*p;
struct NUMBER{ //数字结构体
    string num; //数字
    int dim; //数组维数
    int dim1; //一维数组长度
}number;

TABLE *symbolTable = new TABLE(); //符号表
TABLE *currentTable = symbolTable; //当前符号表

char a[MAX]={'\0'};     //存放源程序
int line=1,i,j,k,l, count=0,v=0,register_no=1, param_dim=0;   //line:行号，i,j,k,l:循环变量，count:标识符个数，v:标识符个数，register_no:寄存器号
int is_global=0;    //是否为全局变量
struct WORD word;    //存放单词
struct ERROR e;     //存放错误
vector<ERROR> v_error;      //存放错误
vector<WORD> v_word;    //存放单词
vector<int> f;      //存放函数参数个数
int cycleDepth=0, return_line=0, last_stmt=0;   //cycleDepth:循环深度，return_line:返回语句所在行，last_stmt:最后一条语句所在行
vector<vector<int>> stack;     //存放循环语句的行号
vector<string> num_stack,output_stack;    //存放数字，存放输出语句
vector<NUMBER> params_stack;   //存放函数参数
vector<string> op_stack;   //存放操作符
vector<string> mid_code;  //存放中间代码
string code,x1,x2, op,stmt_type;  //code:中间代码，x1,x2:操作数，op:操作符，stmt_type:语句类型
int while_count=0, if_count=0;  //while_count:while语句个数，if_count:if语句个数
vector<string> break_stack, continue_stack;   //存放break语句和continue语句的行号
vector<int> upper_bound;   //存放数组的上下界
int  t1=0, t2=0, is_const_number;     //t1:数组维数，t2:数组维数 t3:是否为常数
string array_temp1[MAXN][MAXN];  //存放数组的上下界
vector<string> bound;  //存放数组的上下界
int params_count = 0;  //函数参数个数
string func_name;  //存放函数名
int value_or_address;  //是否为地址 0:值 1:地址
int array_dim=0;  //数组维数
int block_no=0;  //块号
int block1_no=0;  //块号
int cond_type = 0; //条件类型 0:if 1:while

//词法分析的相关函数
bool IsNumber(char x); //判断是否为数字
bool IsAlpha(char x);//判断是否为字母
void lexical_analyzer(string data);  //词法分析
void save_v_word(string name, string type, int line_number); //保存单词

//语法分析的相关函数
void syntactic_analyzer(); //语法分析
void CompUnit();   //程序
void Decl();  //声明
void ConstDecl(); //常量声明
void ConstDef(); //常量定义
void ConstInitVal(); //常量初始化值
void VarDecl(); //变量声明
void VarDef();  //变量定义
void InitVal(); //初始化值
void FuncDef(); //函数定义
void MainFuncDef(); //主函数定义
void FuncType();    //函数类型
void FuncFParams(); //函数形参
void FuncFParam(); //函数形参
void Block();  //复合语句
void BlockItem(); //复合语句中的语句
void Stmt(); //语句
void Exp(); //表达式
void Cond(); //条件
void LVal(); //左值
void PrimaryExp(); //基本表达式
void Number(); //数字
void UnaryExp(); //一元表达式
void UnaryOp(); //一元运算符
void FuncRParams(); //函数实参
void MulExp(); //乘法表达式
void AddExp(); //加法表达式
void RelExp(); //关系表达式
void EqExp(); //等式表达式
void LAndExp(); //逻辑与表达式
void LOrExp(); //逻辑或表达式
void ConstExp(); //常量表达式
bool IsStmt(struct WORD w); //判断是否为语句
bool IsExp(struct WORD w); //判断是否为表达式

//中间代码生成相关函数
int check_num_type(string num);  //检查数字类型
void compute2();  //计算表达式
void compute1();  //计算表达式
void save_array_value(string name, int type, vector<int> upper, string value[MAXN][MAXN],int flag1);  //保存数组
void save_value1(string name,string address, vector<int> upper, string value[MAXN][MAXN]);  //保存数组
void save_value2(string name,string address, vector<int> upper, string value[MAXN][MAXN], int flag);  //保存数组
string get_array(string name, vector<string> upper,int flag);  //获取数组
void backFill_func_param(string name, int func_param_dim, vector<string> c);  //回填函数参数
void backFill_func_end(string name);  //回填函数结束
bool IsNumberString(string x);  //判断是否为数字

//错误处理的相关函数
void error();  //错误处理
void save_v_error(char type, int line_number);  //保存错误
struct SYMBOL* check_symbol(struct WORD w);  //检查符号表
void save_symbol(struct SYMBOL s);  //保存符号表
void SymbolInit(struct SYMBOL s);  //初始化符号表
void check_func_params(struct WORD w, vector<int> z);  //检查函数参数

int main () {
    fstream fs("testfile.txt");
    istreambuf_iterator<char> beg(fs),end;
    string data(beg,end);
    ofstream outfile("output.txt");
    stack.clear();
    streambuf* pOld = cout.rdbuf(outfile.rdbuf());

    for(i = 0; i < data.length();i++){
        if(data[i]=='\n'){
            line++;
        }
        if(data[i]!=' '){
            lexical_analyzer(data);
        }
    }
    syntactic_analyzer();
    cout.rdbuf(pOld);
    fs.close();
    outfile.close();

    ofstream outfile1("error.txt");
    streambuf* pOld1 = cout.rdbuf(outfile1.rdbuf());
    for(i=0;i<v_error.size();i++){
        cout << v_error[i].line_number << " "<< v_error[i].type << endl;
    }
    cout.rdbuf(pOld1);
    outfile1.close();

    ofstream outfile2("llvm_ir.txt");
    streambuf* pOld2 = cout.rdbuf(outfile2.rdbuf());
    for(i=0;i<mid_code.size();i++){
        cout << mid_code[i]<< endl;
    }
    cout.rdbuf(pOld2);
    outfile2.close();
    return 0;
}

void lexical_analyzer(string data){ //词法分析
    //判断标识符
    if(IsAlpha(data[i])||data[i]=='_'){
        k=0;
        for(j=i;j< data.length();j++){
            if(IsNumber(data[j])||IsAlpha(data[j])||data[j]=='_')
                a[k++]=data[j];
            if(!IsNumber(data[j])&&!IsAlpha(data[j])&&data[j]!='_'){
                if(strcmp(a, "const")==0)
                    save_v_word(a, "CONSTTK", line);
                else if(strcmp(a, "main")==0)
                    save_v_word(a, "MAINTK", line);
                else if(strcmp(a, "int")==0)
                    save_v_word(a, "INTTK", line);
                else if(strcmp(a, "break")==0)
                    save_v_word(a, "BREAKTK", line);
                else if(strcmp(a, "continue")==0)
                    save_v_word(a, "CONTINUETK", line);
                else if(strcmp(a, "if")==0)
                    save_v_word(a, "IFTK", line);
                else if(strcmp(a, "else")==0)
                    save_v_word(a, "ELSETK", line);
                else if(strcmp(a, "while")==0)
                    save_v_word(a, "WHILETK", line);
                else if(strcmp(a, "getint")==0)
                    save_v_word(a, "GETINTTK", line);
                else if(strcmp(a, "return")==0)
                    save_v_word(a, "RETURNTK", line);
                else if(strcmp(a, "void")==0)
                    save_v_word(a, "VOIDTK", line);
                else if(strcmp(a, "printf")==0)
                    save_v_word(a, "PRINTFTK", line);
                else if(strcmp(a, "bitand")==0)
                    save_v_word(a, "BITANDTK", line);
                else
                    save_v_word(a, "IDENFR", line);
                i=j-1;
                l = strlen(a);
                for(k=l;k >= 0;k--){
                    a[k]='\0';
                }
                break;
            }
        }
    }
        //判断数字串
    else if(IsNumber(data[i])){
        k=0;
        for(j=i; j < data.length(); j++){
            if(IsNumber(data[j]))
                a[k++]=data[j];
            if(!IsNumber(data[j])){
                save_v_word(a, "INTCON", line);
                i=j-1;
                l = strlen(a);
                for(k=l;k >= 0;k--){
                    a[k]='\0';
                }
                break;
            }
        }
    }
        //判断字符串常量
    else if(data[i]=='"'){
        k=0;
        string s;
        for(j=i;j<data.length();j++){
            s = s + data[j];
            if(data[j]=='"'&&j!=i){
                i=j;
                save_v_word(s, "STRCON", line);
                break;
            }
        }
    }
        //判断！和！=
    else if(data[i]=='!'){
        if(data[i+1]=='='){
            save_v_word("!=", "NEQ", line);
            i=i+1;
        }
        else
            save_v_word("!", "NOT", line);
    }
        //判断=和==
    else if(data[i]=='='){
        if(data[i+1]=='='){
            save_v_word("==", "EQL", line);
            i=i+1;
        }
        else
            save_v_word("=", "ASSIGN", line);
    }
        //判断<和<=
    else if(data[i]=='<'){
        if(data[i+1]=='='){
            save_v_word("<=", "LEQ", line);
            i=i+1;
        }
        else
            save_v_word("<", "LSS", line);
    }
        //判断>和>=
    else if(data[i]=='>'){
        if(data[i+1]=='='){
            save_v_word(">=", "GEQ", line);
            i=i+1;
        }
        else
            save_v_word(">", "GRE", line);
    }
    else if(data[i]=='&'&&data[i+1]=='&'){
        save_v_word("&&", "AND", line);
        i=i+1;
    }
    else if(data[i]=='|'&&data[i+1]=='|'){
        save_v_word("||", "OR", line);
        i=i+1;
    }
    else if(data[i]=='+')
        save_v_word("+", "PLUS", line);
    else if(data[i]=='-')
        save_v_word("-", "MINU", line);
    else if(data[i]=='*')
        save_v_word("*", "MULT", line);
    else if(data[i]=='%')
        save_v_word("%", "MOD", line);
    else if(data[i]==';')
        save_v_word(";", "SEMICN", line);
    else if(data[i]==',')
        save_v_word(",", "COMMA", line);
    else if(data[i]=='(')
        save_v_word("(", "LPARENT", line);
    else if(data[i]==')')
        save_v_word(")", "RPARENT", line);
    else if(data[i]=='[')
        save_v_word("[", "LBRACK", line);
    else if(data[i]==']')
        save_v_word("]", "RBRACK", line);
    else if(data[i]=='{')
        save_v_word("{", "LBRACE", line);
    else if(data[i]=='}')
        save_v_word("}", "RBRACE", line);
    else if(data[i]=='/'){
        if(data[i+1]=='/'){
            for(j=i;j<data.length();j++){
                if(data[j]=='\n'){
                    line++;
                    i=j;
                    break;
                }
            }
        }
        else if(data[i+1]=='*'){
            for(j=i+2;j<data.length();j++){
                if(data[j]=='\n')
                    line++;
                else if(data[j]=='*'&&data[j+1]=='/'){
                    i=j+1;
                    break;
                }
            }
        }
        else
            save_v_word("/", "DIV", line);
    }
}
void syntactic_analyzer(){ //语法分析
    i=0;
    CompUnit();
}

void CompUnit(){ //程序
    while(v_word[i].name=="const"||(v_word[i].name=="int"&&v_word[i+2].name!="(")){
        is_global=1;
        Decl();
        is_global=0;
    }
    code = "declare i32 @getint()";
    mid_code.push_back(code);
    code = "declare void @putint(i32)";
    mid_code.push_back(code);
    code = "declare void @putch(i32)";
    mid_code.push_back(code);
    code = "declare void @memset(i32*, i32, i32)";
    mid_code.push_back(code);
    while((v_word[i].name=="void"||v_word[i].name=="int")&&v_word[i+1].name!="main"){
        FuncDef();
    }
    if(v_word[i].name=="int"&&v_word[i+1].name=="main"){
        MainFuncDef();
        cout <<"<CompUnit>" << endl;
    }
    else error();
}
void Decl(){
    if(v_word[i].name=="const") {
        ConstDecl();
    }
    else {
        VarDecl();
    }
}
void ConstDecl(){
    cout << v_word[i].type << " " << v_word[i].name << endl;
    i++;
    if(v_word[i].name=="int"){
        cout << v_word[i].type << " " << v_word[i].name << endl;
        i++;
        if(v_word[i].type=="IDENFR"){
            ConstDef();
            while(v_word[i].name==","){
                cout << v_word[i].type << " " << v_word[i].name << endl;
                i++;
                ConstDef();
            }
            if(v_word[i].name==";"){
                cout << v_word[i].type << " " << v_word[i].name << endl;
                i++;
                cout << "<ConstDecl>" << endl;
            }
            else
                save_v_error('i', v_word[i-1].line_number);
        }
        else error();
    }
    else error();
}
void ConstDef(){
    upper_bound.clear();
    t1=0;
    t2=0;
    if(v_word[i].type=="IDENFR"){
        if(check_symbol(v_word[i])!=NULL){
            save_v_error('b', v_word[i].line_number);
        }
        struct SYMBOL symbol;
        SymbolInit(symbol);
        symbol.name=v_word[i].name;
        cout << v_word[i].type << " " << v_word[i].name << endl;
        i++;
        count=0;
        while(v_word[i].name=="["){
            count++;
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            is_const_number=1;
            ConstExp();
            is_const_number=0;
            upper_bound.push_back(stoi(num_stack.back()));
            num_stack.pop_back();
            if(v_word[i].name=="]"){
                cout << v_word[i].type << " " << v_word[i].name << endl;
                i++;
            }
            else
                save_v_error('k', v_word[i-1].line_number);
        }
        if(is_global!=1){
            if(count==1){
                symbol.dim1=upper_bound[0];
                code = "\t%"+ to_string(register_no)+" = alloca ["+to_string(upper_bound[0])+" x i32]";
                symbol.address="%"+ to_string(register_no);
                register_no++;
                mid_code.push_back(code);
            }
            else if(count==2){
                symbol.dim1=upper_bound[0];
                symbol.dim2=upper_bound[1];
                code = "\t%"+ to_string(register_no)+" = alloca ["+to_string(upper_bound[0])+" x ["+to_string(upper_bound[1])+" x i32]]";
                symbol.address="%"+ to_string(register_no);
                register_no++;
                mid_code.push_back(code);
            }
        }
        if(is_global==1){
            if(count==1){
                symbol.dim1=upper_bound[0];
                symbol.address="@"+ symbol.name;
            }
            else if(count==2){
                symbol.dim1=upper_bound[0];
                symbol.dim2=upper_bound[1];
                symbol.address="@"+ symbol.name;
            }
        }
        symbol.dim=count;
        symbol.type="ConstNumber";
        int flag1=0;
        if(v_word[i].name=="="){
            flag1=1;
            if(count == 1){
                for(int i1=0;i1<upper_bound[0];i1++){
                    array_temp1[0][i1]="0";
                }
            }
            if(count == 2){
                for(int i1=0;i1<upper_bound[0];i1++){
                    for(int i2=0;i2<upper_bound[1];i2++){
                        array_temp1[i1][i2] = "0";
                    }
                }
            }
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            ConstInitVal();
            cout<<"<ConstDef>"<< endl;
        }
        else error();
        save_symbol(symbol);
        printf("is_global=%d\n", is_global);
        if(is_global==1){
            save_array_value(symbol.name, 1, upper_bound, array_temp1,flag1);
            upper_bound.clear();
        }
        else
            save_value1(symbol.name, symbol.address, upper_bound, array_temp1);
    }
    else error();
}
void ConstInitVal(){
    if(IsExp(v_word[i])){
        is_const_number=1;
        ConstExp();
        is_const_number=0;
        array_temp1[t1][t2] = num_stack.back();
        num_stack.pop_back();
        t2++;
        cout <<"<ConstInitVal>" << endl;
    }
    else if(v_word[i].name=="{"){
        cout << v_word[i].type << " " << v_word[i].name << endl;
        i++;
        if(IsExp(v_word[i])||v_word[i].name=="{"){
            ConstInitVal();
            while(v_word[i].name==","){
                cout << v_word[i].type << " " << v_word[i].name << endl;
                i++;
                ConstInitVal();
            }
        }
        if(v_word[i].name=="}"){
            t1++;
            t2=0;
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            cout << "<ConstInitVal>" << endl;
        }
        else error();
    }
    else error();
}
void VarDecl(){
    if(v_word[i].name=="int"){
        cout << v_word[i].type << " " << v_word[i].name << endl;
        i++;
        VarDef();
        while(v_word[i].name==","){
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            VarDef();
        }
        if(v_word[i].name==";"){
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            cout << "<VarDecl>" << endl;
        }
        else
            save_v_error('i', v_word[i-1].line_number);
    }
    else error();
}
void VarDef(){
    upper_bound.clear();
    t1=0;
    t2=0;
    if(v_word[i].type=="IDENFR"){
        if(check_symbol(v_word[i])!=NULL){
            save_v_error('b', v_word[i].line_number);
        }
        struct SYMBOL symbol;
        SymbolInit(symbol);
        symbol.name=v_word[i].name;
        cout << v_word[i].type << " " << v_word[i].name << endl;
        i++;
        count=0;
        while(v_word[i].name=="["){
            count++;
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            is_const_number=1;
            ConstExp();
            is_const_number=0;
            printf("%s\n",num_stack.back().c_str());
            upper_bound.push_back(stoi(num_stack.back()));
            num_stack.pop_back();
            if(v_word[i].name=="]"){
                cout << v_word[i].type << " " << v_word[i].name << endl;
                i++;
            }
            else
                save_v_error('k', v_word[i-1].line_number);
        }
        if(is_global!=1){
            if(count==1){
                symbol.dim1=upper_bound[0];
                code = "\t%"+ to_string(register_no)+" = alloca ["+to_string(upper_bound[0])+" x i32]";
                symbol.address="%"+ to_string(register_no);
                register_no++;
                mid_code.push_back(code);
            }
            else if(count==2){
                symbol.dim1=upper_bound[0];
                symbol.dim2=upper_bound[1];
                code = "\t%"+ to_string(register_no)+" = alloca ["+to_string(upper_bound[0])+" x ["+to_string(upper_bound[1])+" x i32]]";
                symbol.address="%"+ to_string(register_no);
                register_no++;
                mid_code.push_back(code);
            }
            else if(count==0){
                code = "\t%"+ to_string(register_no)+" = alloca i32";
                symbol.address="%"+ to_string(register_no);
                register_no++;
                mid_code.push_back(code);
            }
        }
        if(is_global==1){
            if(count==1){
                symbol.dim1=upper_bound[0];
                symbol.address="@"+ symbol.name;
            }
            else if(count==2){
                symbol.dim1=upper_bound[0];
                symbol.dim2=upper_bound[1];
                symbol.address="@"+ symbol.name;
            }
            else if(count==0){
                symbol.address="@"+ symbol.name;
            }
        }
        symbol.dim=count;
        symbol.type="VarNumber";
        int flag =0;
        if(count == 1){
            for(int i1=0;i1<upper_bound[0];i1++){
                symbol.arr[0][i1]="";
            }
        }
        if(count == 2){
            for(int i1=0;i1<upper_bound[0];i1++){
                for(int i2=0;i2<upper_bound[1];i2++){
                    symbol.arr[i1][i2]="";
                }
            }
        }
        if(v_word[i].name=="="){
            if(count == 1){
                for(int i1=0;i1<upper_bound[0];i1++){
                    array_temp1[0][i1]="0";
                }
            }
            if(count == 2){
                for(int i1=0;i1<upper_bound[0];i1++){
                    for(int i2=0;i2<upper_bound[1];i2++){
                        array_temp1[i1][i2] = "0";
                    }
                }
            }
            flag = 1;
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            if(v_word[i].name=="getint"&&v_word[i+1].name=="("){
                i=i+2;
                if(v_word[i].name==")"){
                    cout << v_word[i].type << " " << v_word[i].name << endl;
                    i++;
                    code = "\t%"+ to_string(register_no)+" = call i32 @getint()";
                    mid_code.push_back(code);
                    register_no++;
                    array_temp1[0][0] = "%"+ to_string(register_no-1);
                }
                else
                    save_v_error('l', v_word[i-1].line_number);
            }
            else{
                InitVal();
            }
            if(upper_bound.size()==0&&is_global!=1){
                symbol.value = array_temp1[0][0];
                code = "\tstore i32 "+symbol.value+", i32* "+symbol.address;
                mid_code.push_back(code);
            }
        }
        cout << "<VarDef>" << endl;
        save_symbol(symbol);
        printf("varDef is_global = %d\n",is_global);
        printf("value = %s\n",symbol.value.c_str());
        if(is_global==1){
            save_array_value(symbol.name, 0, upper_bound, array_temp1,flag);
        }
        else if(flag==1){
            save_value2(symbol.name, symbol.address, upper_bound, array_temp1, flag);
        }
    }
    else error();
}
void InitVal(){
    if(IsExp(v_word[i])){
        Exp();
        while(!op_stack.empty()&&op_stack.back()!="("){
            if(is_global==1) compute1();
            else compute2();
        }
        if(is_global==1){
            array_temp1[t1][t2] = num_stack.back();
            num_stack.pop_back();
            t2++;
        }
        else{
            array_temp1[t1][t2] = num_stack.back();
            num_stack.pop_back();
            t2++;
        }
        cout << "<InitVal>" << endl;
    }
    else if(v_word[i].name=="{"){
        cout << v_word[i].type << " " << v_word[i].name << endl;
        i++;
        if(IsExp(v_word[i])||v_word[i].name=="{"){
            InitVal();
            while(v_word[i].name==","){
                cout << v_word[i].type << " " << v_word[i].name << endl;
                i++;
                InitVal();
            }
        }
        if(v_word[i].name=="}"){
            t1++;
            t2=0;
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            cout << "<InitVal>" << endl;
        }
        else error();
    }
    else error();
}
void FuncDef(){
    register_no=0;
    if(v_word[i].name=="void"||v_word[i].name=="int"){
        FuncType();
        if(v_word[i].type=="IDENFR"){
            func_name = v_word[i].name;
            int i2 = i;
            if(check_symbol(v_word[i])!=NULL){
                save_v_error('b', v_word[i].line_number);
            }
            struct SYMBOL symbol;
            SymbolInit(symbol);
            symbol.type=v_word[i-1].name+"Func";
            if(v_word[i-1].name=="void") {
                symbol.dim=-1;
                code = "define void @"+v_word[i2].name+"(";
                mid_code.push_back(code);
            }
            else if(v_word[i-1].name=="int") {
                symbol.dim=0;
                code = "define i32 @"+v_word[i2].name+"(";
                mid_code.push_back(code);
            }
            for(int i1 = i; v_word[i1].name!="{"; i1++){
                if(v_word[i1].name=="int"){
                    register_no++;
                }
            }
            string s = v_word[i].name;
            symbol.name=v_word[i].name;
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            save_symbol(symbol);
            if(v_word[i].name=="("){
                f.clear();
                symbolTable = new TABLE();
                symbolTable->parent = currentTable;
                currentTable = symbolTable;
                cout << v_word[i].type << " " << v_word[i].name << endl;
                i++;
                register_no++;
                if(v_word[i].name=="int"){
                    FuncFParams();
                }
                backFill_func_end(func_name);
                if(v_word[i].name==")"){
                    cout << v_word[i].type << " " << v_word[i].name << endl;
                    i++;
                }
                else
                    save_v_error('j', v_word[i-1].line_number);
                symbol=currentTable->parent->symbol[s];
                symbol.func_params = f;
                p=currentTable->parent;
                p->symbol[symbol.name]=symbol;
                if(v_word[i].name=="{"){
                    return_line=0;
                    last_stmt=0;
                    Block();
                    p=currentTable->parent;
                    if(p->symbol[v_word[i2].name].type=="voidFunc"&&return_line != 0)
                        save_v_error('f',return_line);
                    if(p->symbol[v_word[i2].name].type=="intFunc"&&last_stmt!=2){
                        save_v_error('g', v_word[i-1].line_number);
                    }
                    cout << "<FuncDef>" << endl;
                    currentTable = currentTable->parent;
                }
                else error();
            }
            else error();
            if(v_word[i2-1].name=="void"){
                mid_code.push_back("\tret void");
            }
            if(mid_code.back().find("ret") == string::npos){
                mid_code.push_back("\tret i32 0");
            }
        }
        else error();
    }
    else error();
    code = "}\n";
    mid_code.push_back(code);
}
void MainFuncDef(){
    register_no=1;
    code = "define i32 @main() {";
    mid_code.push_back(code);
    if(v_word[i].name=="int"&&v_word[i+1].name=="main"&&
       v_word[i+2].name=="("){
        cout << v_word[i].type << " " << v_word[i].name << endl;
        cout << v_word[i+1].type << " " << v_word[i+1].name << endl;
        cout << v_word[i+2].type << " " << v_word[i+2].name << endl;
        if(check_symbol(v_word[i+1])!=NULL){
            save_v_error('b', v_word[i+1].line_number);
        }
        struct SYMBOL symbol;
        SymbolInit(symbol);
        symbol.type="mainFunc";
        symbol.dim=0;
        symbol.name=v_word[i+1].name;
        save_symbol(symbol);

        i=i+3;
        if(v_word[i].name==")"){
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
        }
        else
            save_v_error('j', v_word[i-1].line_number);
        if(v_word[i].name=="{"){
            return_line=0;
            Block();
            if(last_stmt!=2){
                save_v_error('g', v_word[i-1].line_number);
            }

            cout << "<MainFuncDef>" << endl;
        }
        else error();
    }
    else error();
    code = "}";
    mid_code.push_back(code);
}
void FuncType(){
    if(v_word[i].name=="int"||v_word[i].name=="void"){
        cout << v_word[i].type << " " << v_word[i].name << endl;
        i++;
        cout <<"<FuncType>" << endl;
    }
    else error();
}
void FuncFParams(){
    if(v_word[i].name=="int"){
        params_count = 0;
        FuncFParam();
        while(v_word[i].name==","){
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            FuncFParam();
        }
        cout << "<FuncFParams>" << endl;
    }
    else error();
}
void FuncFParam(){
    vector<string> c;
    if(v_word[i].name=="int"){
        cout << v_word[i].type << " " << v_word[i].name << endl;
        i++;
        if(v_word[i].type=="IDENFR"){
            if(check_symbol(v_word[i])!=NULL){
                save_v_error('b', v_word[i].line_number);
            }
            struct SYMBOL symbol;
            SymbolInit(symbol);
            symbol.type = "ParamNumber";
            symbol.name = v_word[i].name;
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            count =0;
            if(v_word[i].name=="["&&v_word[i+1].name=="]"){
                count++;
                cout << v_word[i].type << " " << v_word[i].name << endl;
                cout << v_word[i+1].type << " " << v_word[i+1].name << endl;
                i=i+2;
                while(v_word[i].name=="["){
                    count++;
                    cout << v_word[i].type << " " << v_word[i].name << endl;
                    i++;
                    is_const_number=1;
                    ConstExp();
                    is_const_number=0;
                    c.push_back(num_stack.back());
                    num_stack.pop_back();
                    if(v_word[i].name=="]"){
                        cout << v_word[i].type << " " << v_word[i].name << endl;
                        i++;
                    }
                    else
                        save_v_error('k', v_word[i-1].line_number);
                }
            }
            else if(v_word[i].name=="["&&v_word[i+1].name!="]")
                save_v_error('k', v_word[i].line_number);
            backFill_func_param(func_name, count, c);
            f.push_back(count);
            symbol.dim=count;
            if(symbol.dim==0){
                code = "\t%"+to_string(register_no)+" = alloca i32";
                mid_code.push_back(code);
                symbol.address = "%"+to_string(register_no);
                code = "\tstore i32 %"+to_string(params_count)+", i32* %"+to_string(register_no);
                params_count++;
                mid_code.push_back(code);
            }
            else if(symbol.dim==1){
                symbol.dim1 = 0;
                code = "\t%"+to_string(register_no)+" = alloca i32*";
                mid_code.push_back(code);
                symbol.address = "%"+to_string(register_no);
                code = "\tstore i32* %"+to_string(params_count)+", i32** %"+to_string(register_no);
                params_count++;
                mid_code.push_back(code);
            }
            else{
                symbol.dim1 = 0;
                symbol.dim2 = stoi(c[0]);
                code = "\t%"+to_string(register_no)+" = alloca ["+c[0]+" x i32]*";
                mid_code.push_back(code);
                symbol.address = "%"+to_string(register_no);
                code = "\tstore ["+c[0]+" x i32]* %"+to_string(params_count)+", ["+c[0]+" x i32]** %"+to_string(register_no);
                params_count++;
                mid_code.push_back(code);
            }
            symbol.address = "%"+to_string(register_no);
            register_no++;
            save_symbol(symbol);
            cout << "<FuncFParam>" << endl;
        }
        else error();
    }
    else error();
}
void Block(){
    if(v_word[i].name=="{"){
        symbolTable = new TABLE();
        symbolTable->parent = currentTable;
        currentTable = symbolTable;

        cout << v_word[i].type << " " << v_word[i].name << endl;
        i++;
        while(v_word[i].name=="int"||v_word[i].name=="const"|| IsStmt(v_word[i])){
            BlockItem();
        }
        if(v_word[i].name=="}"){
            cout << v_word[i].type << " " << v_word[i].name << endl;
            currentTable = currentTable->parent;
            i++;
            cout << "<Block>" << endl;
        }
        else
            error();
    }
    else error();
}
void BlockItem(){
    if(v_word[i].name=="int"||v_word[i].name=="const"){
        Decl();
    }
    else Stmt();
}
void Stmt(){
    if(v_word[i].name=="printf"){
        last_stmt=1;
        if(v_word[i+1].name=="("){
            if(v_word[i+2].type=="STRCON"){
                int i2 = i+2;
                cout << v_word[i].type << " " << v_word[i].name << endl;
                cout << v_word[i+1].type << " " << v_word[i+1].name << endl;
                cout << v_word[i+2].type << " " << v_word[i+2].name << endl;
                string s = v_word[i+2].name;
                int len = s.length(), flag=0;
                for(j = 1;j < len-1; j++){
                    if(s[j]==92&&s[j+1]!='n'){
                        flag=1;
                        break;
                    }
                    else if(s[j]=='%'&&s[j+1]!='d'){
                        flag=1;
                        break;
                    }
                    else if(s[j]==92&&s[j+1]=='n'){
                        j++;
                    }
                    else if(s[j]=='%'&&s[j+1]=='d'){
                        j++;
                    }
                    else if(s[j]!=32&&s[j]!=33&&(s[j]<40||s[j]>126)&&s[j]!=37){
                        flag=1;
                        break;
                    }
                }
                if(flag==1)
                    save_v_error('a', v_word[i+2].line_number);
                i=i+3;
                output_stack.clear();
                int count_1 = 0, count_2=0;
                while(v_word[i].name==","){
                    count_1++;
                    cout << v_word[i].type << " " << v_word[i].name << endl;
                    i++;
                    Exp();
                    while(!op_stack.empty()&&op_stack.back()!="("){
                        if(is_global==1) compute1();
                        else compute2();
                    }
                    output_stack.push_back(num_stack.back());
                    num_stack.pop_back();
                }
                for(j =1;j<len-1;j++){
                    if(s[j]=='%'&&s[j+1]=='d'){
                        code = "\tcall void @putint(i32 "+output_stack[count_2]+")";
                        mid_code.push_back(code);
                        count_2++;
                        j++;
                    }
                    else if(s[j]=='\\'&&s[j+1]=='n'){
                        code = "\tcall void @putch(i32 10)";
                        mid_code.push_back(code);
                        j++;
                    }
                    else{
                        code = "\tcall void @putch(i32 "+to_string(s[j])+")";
                        mid_code.push_back(code);
                    }
                }
                if(count_1!=count_2)
                    save_v_error('l',v_word[i2-2].line_number);
                if(v_word[i].name==")"){
                    cout << v_word[i].type << " " << v_word[i].name << endl;
                    i=i+1;
                }
                else
                    save_v_error('j', v_word[i-1].line_number);
                if(v_word[i].name==";"){
                    cout << v_word[i].type << " " << v_word[i].name << endl;
                    i=i+1;
                    cout << "<Stmt>" << endl;
                }
                else
                    save_v_error('i', v_word[i-1].line_number);
            }
            else error();
        }
        else error();
    }
    else if(v_word[i].name=="return"){
        cout << v_word[i].type << " " << v_word[i].name << endl;
        i++;
        last_stmt=7;
        if(IsExp(v_word[i])){
            last_stmt=2;
            return_line=v_word[i-1].line_number;
            Exp();
            while(!op_stack.empty()){
                if(is_global) compute1();
                else compute2();
            }
        }
        if(v_word[i].name==";"){
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            register_no++;
            cout << "<Stmt>" << endl;
        }
        else
            save_v_error('i', v_word[i-1].line_number);
        if(last_stmt==2){
            x1 = num_stack.back();
            code = "\tret i32 "+ x1;
            mid_code.push_back(code);
            num_stack.pop_back();
        }
        else {
            code = "\tret void";
            mid_code.push_back(code);
        }
    }
    else if(v_word[i].name=="break"||v_word[i].name=="continue"){
        last_stmt=3;
        if(v_word[i].name=="break"){
            if(break_stack.empty())
                save_v_error('m', v_word[i].line_number);
            else{
                code = "\tbr label %"+break_stack.back();
                mid_code.push_back(code);
                register_no++;
            }
        }
        else{
            if(continue_stack.empty())
                save_v_error('m', v_word[i].line_number);
            else{
                code = "\tbr label %"+continue_stack.back();
                mid_code.push_back(code);
                register_no++;
            }
        }
        cout << v_word[i].type << " " << v_word[i].name << endl;
        i++;
        if(v_word[i].name==";"){
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            cout << "<Stmt>" << endl;
        }
        else
            save_v_error('i', v_word[i-1].line_number);
    }
    else if(v_word[i].name=="while"){
        cycleDepth++;
        if(v_word[i+1].name=="("&& IsExp(v_word[i+2])){
            cout << v_word[i].type << " " << v_word[i].name << endl;
            cout << v_word[i+1].type << " " << v_word[i+1].name << endl;
            i=i+2;
            int while_no = while_count;
            while_count++;
            code = "\tbr label %while"+to_string(while_no);
            mid_code.push_back(code);
            code = "\nwhile"+to_string(while_no)+":"; //while1:
            mid_code.push_back(code);
            continue_stack.push_back("while"+to_string(while_no));
            break_stack.push_back("while"+to_string(while_no)+"_end");
            Cond();
            if(check_num_type(num_stack.back())!=1){
                string temp = num_stack.back();
                num_stack.pop_back();
                code = "\t%"+ to_string(register_no)+" = icmp ne i32 "+temp+", 0";
                mid_code.push_back(code);
                num_stack.push_back("%"+to_string(register_no));
                register_no++;
            }
            if(cond_type==1){
                code = "\tbr i1 "+num_stack.back()+", label %block_exec"+ to_string(block1_no)+", label %block_out"+to_string(block1_no);
                mid_code.push_back(code);
                code = "\nblock_exec"+ to_string(block1_no)+":";
                mid_code.push_back(code);
                code = "\tbr label %while"+to_string(while_no)+"_body";
                mid_code.push_back(code);
                code = "\nblock_out"+ to_string(block1_no)+":";
                mid_code.push_back(code);
                code = "\tbr label %while"+to_string(while_no)+"_end";
                mid_code.push_back(code);
            }
            else{
                code="\tbr i1 "+num_stack.back()+", label %while"+to_string(while_no)+"_body, label %while"+to_string(while_no)+"_end";
                mid_code.push_back(code);
            }
            num_stack.pop_back();
            if(v_word[i].name==")"&& IsStmt(v_word[i+1])){
                cout << v_word[i].type << " " << v_word[i].name << endl;
                i=i+1;
                code = "\nwhile"+to_string(while_no)+"_body:"; //while1_body:
                mid_code.push_back(code);
                Stmt();
                code = "\tbr label %while"+to_string(while_no);
                mid_code.push_back(code);
                code = "\nwhile"+to_string(while_no)+"_end:"; //while1_end:
                mid_code.push_back(code);
                cout << "<Stmt>" << endl;
                break_stack.pop_back();
                continue_stack.pop_back();
                cycleDepth--;
            }
            else if(v_word[i].name!=")")
                save_v_error('j', v_word[i-1].line_number);
        }
        else error();
    }
    else if(v_word[i].name=="if"){
        if(v_word[i+1].name=="("&& IsExp(v_word[i+2])) {
            cout << v_word[i].type << " " << v_word[i].name << endl;
            cout << v_word[i+1].type << " " << v_word[i+1].name << endl;
            i = i + 2;
            int if_no = if_count;
            Cond();
            if_count++;
            if(check_num_type(num_stack.back())!=1){
                string temp = num_stack.back();
                num_stack.pop_back();
                code = "\t%"+ to_string(register_no)+" = icmp ne i32 "+temp+", 0";
                mid_code.push_back(code);
                num_stack.push_back("%"+to_string(register_no));
                register_no++;
            }
            if(cond_type==1){
                code = "\tbr i1 "+num_stack.back()+", label %block_exec"+ to_string(block1_no)+", label %block_out"+to_string(block1_no);
                mid_code.push_back(code);
                code = "\nblock_exec"+ to_string(block1_no)+":";
                mid_code.push_back(code);
                code = "\tbr label %label"+to_string(if_no)+"_1";
                mid_code.push_back(code);
                code = "\nblock_out"+ to_string(block1_no)+":";
                mid_code.push_back(code);
                code = "\tbr label %label"+to_string(if_no)+"_2";
                mid_code.push_back(code);
            }
            else{
                code = "\tbr i1 "+num_stack.back()+", label %label"+to_string(if_no)+"_1, label %label"+to_string(if_no)+"_2";
                mid_code.push_back(code);
            }
            num_stack.pop_back();
            if(v_word[i].name==")"&& IsStmt(v_word[i+1])){
                cout << v_word[i].type << " " << v_word[i].name << endl;
                i=i+1;
                code = "\nlabel"+ to_string(if_no)+"_1:";
                mid_code.push_back(code);
                Stmt();
                if(v_word[i].name=="else"){
                    cout << v_word[i].type << " " << v_word[i].name << endl;
                    i++;
                    code = "\tbr label %label"+ to_string(if_no)+"_3";
                    mid_code.push_back(code);
                    code = "\nlabel"+ to_string(if_no)+"_2:";
                    mid_code.push_back(code);
                    Stmt();
                    code = "\tbr label %label"+ to_string(if_no)+"_3";
                    mid_code.push_back(code);
                    code = "\nlabel"+ to_string(if_no)+"_3:";
                    mid_code.push_back(code);
                }
                else{
                    code = "\tbr label %label"+ to_string(if_no)+"_2";
                    mid_code.push_back(code);
                    code = "\nlabel"+ to_string(if_no)+"_2:";
                    mid_code.push_back(code);
                }
                cout << "<Stmt>" << endl;
            }
            else if(v_word[i].name!=")")
                save_v_error('j', v_word[i-1].line_number);
        }
        else error();
    }
    else if(v_word[i].name=="{"){
        Block();
        cout << "<Stmt>" << endl;
    }
    else {
        j=i;
        int c=0;
        int line1=v_word[j].line_number;
        while(v_word[j].line_number==line1){
            if(v_word[j].name=="="){
                if(v_word[j+1].name=="getint") c=1;
                else c=2;
                break;
            }
            j++;
        }
        if(c==1){
            stmt_type="1";
            last_stmt=4;
            p=currentTable;
            int flag=0;
            struct SYMBOL *s = check_symbol(v_word[i]);
            if(s!=NULL&&s->type=="ConstNumber"){
                save_v_error('h', v_word[i].line_number);
            }
            value_or_address = 1;
            LVal();
            value_or_address = 0;
            string temp = num_stack.back();
            stmt_type.clear();
            code = "\t%"+ to_string(register_no) +" = call i32 @getint()";
            mid_code.push_back(code);
            num_stack.push_back("%"+to_string(register_no));
            s->value = "%"+to_string(register_no);
            if(s->dim==0)
                code = "\tstore i32 "+s->value+", i32* "+s->address;
            else
                code = "\tstore i32 "+s->value+", i32* "+temp;
            mid_code.push_back(code);
            register_no++;
            if(v_word[i].name=="=" && v_word[i+1].name=="getint" &&
               v_word[i+2].name=="(" ){
                cout << v_word[i].type << " " << v_word[i].name << endl;
                cout << v_word[i+1].type << " " << v_word[i+1].name << endl;
                cout << v_word[i+2].type << " " << v_word[i+2].name << endl;
                i=i+3;
                cout << "<Stmt>" << endl;
            }
            if(v_word[i].name==")"){
                cout << v_word[i].type << " " << v_word[i].name << endl;
                i++;
            }
            else
                save_v_error('j', v_word[i-1].line_number);
            if(v_word[i].name==";"){
                cout << v_word[i].type << " " << v_word[i].name << endl;
                i++;
                cout << "<Stmt>" << endl;
            }
            else
                save_v_error('i', v_word[i-1].line_number);
        }
        if(c==2){
            last_stmt=5;
            stmt_type="2";
            p=currentTable;
            int temp = i;
            int flag=0;
            while(p!=NULL){
                if(p->symbol.count(v_word[i].name)!=0){
                    if(p->symbol[v_word[i].name].type=="ConstNumber") {
                        flag=1;
                        break;
                    }
                }
                p=p->parent;
            }
            if(flag==1){
                save_v_error('h', v_word[i].line_number);
            }
            string s = v_word[i].name;
            value_or_address = 1;
            LVal();
            value_or_address = 0;
            string temp1 = num_stack.back();
            num_stack.pop_back();
            stmt_type.clear();
            if(v_word[i].name=="=" && IsExp(v_word[i+1])){
                cout << v_word[i].type << " " << v_word[i].name << endl;
                i++;
                Exp();
                while(!op_stack.empty()&&op_stack.back()!="("){
                    if(is_global==1) compute1();
                    else compute2();
                }
                if(v_word[i].name==";"){
                    cout << v_word[i].type << " " << v_word[i].name << endl;
                    i++;
                    struct SYMBOL *symbol4 = check_symbol(v_word[temp]);
                    symbol4->value = num_stack.back();
                    num_stack.pop_back();
                    if(symbol4->dim==0)
                        code = "\tstore i32 "+symbol4->value+", i32* "+symbol4->address;
                    else
                        code = "\tstore i32 "+symbol4->value+", i32* "+temp1;
                    mid_code.push_back(code);
                    cout << "<Stmt>" << endl;
                }
                else
                    save_v_error('i', v_word[i-1].line_number);
            }
            else error();
        }
        if(c==0){
            last_stmt=6;
            if(IsExp(v_word[i])){
                Exp();
                while(!op_stack.empty()){
                    if(is_global) compute1();
                    else compute2();
                }
                num_stack.clear();
            }
            if(v_word[i].name==";"){
                cout << v_word[i].type << " " << v_word[i].name << endl;
                i++;
                cout << "<Stmt>" << endl;
            }
            else
                save_v_error('i', v_word[i-1].line_number);
        }
    }
}
void Exp(){
    if(IsExp(v_word[i])){
        AddExp();
        cout << "<Exp>" << endl;
    }
    else error();
}
void Cond(){
    if(IsExp(v_word[i])){
        block1_no++;
        cond_type=0;
        LOrExp();
        cout << "<Cond>" << endl;
    }
    else error();
}
void LVal(){
    vector<string> temp_num_stack;
    vector<string> temp_op_stack;
    temp_num_stack = num_stack;
    temp_op_stack = op_stack;
    num_stack.clear();
    op_stack.clear();
    vector<string> temp_bound;
    temp_bound = bound;
    bound.clear();
    if(v_word[i].type=="IDENFR"){
        cout << v_word[i].type << " " << v_word[i].name << endl;
        if(check_symbol(v_word[i])==NULL){
            save_v_error('c', v_word[i].line_number);
        }
        else{
            struct SYMBOL *symbol2 = check_symbol(v_word[i]);
            if(symbol2->type=="voidFunc"||symbol2->type=="intFunc"){
                save_v_error('c', v_word[i].line_number);
            }
        }
        i++;
        int q=0;
        int i1 = i-1;
        while(v_word[i].name=="["){
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            q++;
            v=1;
            int flag1=0;
            for(int i2 = i;v_word[i2].name!="]";i2++){
                //不是数字或者运算符
                if(v_word[i2].type!="INTCON"&&v_word[i2].name!="+"&&v_word[i2].name!="-"&&v_word[i2].name!="*"&&v_word[i2].name!="/"&&v_word[i2].name!="("&&v_word[i2].name!=")"&&v_word[i2].name!="%"){
                    printf("v_word[%d].name=%s v_word[%d].type=%s\n", i2, v_word[i2].name.c_str(), i2, v_word[i2].type.c_str());
                    if(v_word[i2].type=="IDENFR"){
                        struct SYMBOL *symbol3 = check_symbol(v_word[i2]);
                        printf("symbol3->type=%s symbol3->value = %s\n", symbol3->type.c_str(), symbol3->value.c_str());
                        if(symbol3->type!="ConstNumber"||symbol3->value==""){
                            flag1=1;
                            break;
                        }
                    }
                }
            }
            printf("flag1 = %d\n", flag1);
            if(flag1==0) is_const_number=1;
            int temp_value_or_address = value_or_address;
            value_or_address = 0;
            Exp();
            value_or_address = temp_value_or_address;
            //输出num_stack中的内容
            for(int i2 = 0;i2<num_stack.size();i2++){
                printf("num_stack[%d]=%s\n", i2, num_stack[i2].c_str());
            }
            while(!op_stack.empty()){
                printf("is_const_number = %d\n", is_const_number);
                if(is_global==1||is_const_number==1) compute1();
                else compute2();
            }
            is_const_number=0;
            printf("num_stack.back()=%s\n", num_stack.back().c_str());
            bound.push_back(num_stack.back());
            num_stack.pop_back();
            v=0;
            if(v_word[i].name=="]"){
                cout << v_word[i].type << " " << v_word[i].name << endl;
                i++;
            }
            else
                save_v_error('k', v_word[i-1].line_number);
        }
        struct SYMBOL *symbol3 = check_symbol(v_word[i1]);
        if(symbol3!=NULL&&!stack.empty()) {
            stack.back().push_back(symbol3->dim-q);
            printf("name = %s dim = %d\n", symbol3->name.c_str(), symbol3->dim);
            printf("aa1\n");
            param_dim = symbol3->dim-q;
            array_dim = symbol3->dim2;
        }
        num_stack = temp_num_stack;
        op_stack = temp_op_stack;
        if(symbol3->dim==0){
            if(symbol3->type!="ConstNumber"){
                printf("name = %s\n", symbol3->name.c_str());
                printf("valuemm = %s\n", symbol3->value.c_str());
                code = "\t%"+to_string(register_no)+" = load i32, i32* "+symbol3->address;
                mid_code.push_back(code);
                num_stack.push_back("%"+to_string(register_no));
                register_no++;
            }
            else if(symbol3->type=="ConstNumber"){
                printf("name = %s\n", symbol3->name.c_str());
                printf("valueqqq = %s\n", symbol3->value.c_str());
                num_stack.push_back(symbol3->value);
            }else {
                num_stack.push_back(symbol3->address);
            }
        }
        else{
            string temp1;
            printf("name = %s\n", symbol3->name.c_str());
            printf("bound.size = %d\n", bound.size());
            for(int i2=0;i2<bound.size();i2++){
                printf("bound[%d] = %s\n", i2, bound[i2].c_str());
            }
            printf("value_or_address = %d\n", value_or_address);
            temp1 = get_array(symbol3->name, bound, value_or_address);
            printf("temp1=%s\n", temp1.c_str());
            num_stack.push_back(temp1);
        }
        bound = temp_bound;
        cout << "<LVal>" << endl;
    }
    else error();
}
void PrimaryExp(){
    if(v_word[i].name=="("){
        cout << v_word[i].type << " " << v_word[i].name << endl;
        op_stack.push_back(v_word[i].name);
        i++;
        Exp();
        if(v_word[i].name==")"){
            cout << v_word[i].type << " " << v_word[i].name << endl;
            while(op_stack.back()!="("&&!op_stack.empty()){
                if(is_global==1||is_const_number==1) compute1();
                else compute2();
            }
            if(op_stack.back()=="("){
                op_stack.pop_back();
            }
            i++;
            cout << "<PrimaryExp>" << endl;
        }
        else error();
    }
    else if(v_word[i].type=="IDENFR"){

        LVal();
        cout << "<PrimaryExp>" << endl;
    }
    else if(IsNumberString(v_word[i].name)){
        Number();
        if(!stack.empty()&&v!=1) {
            stack.back().push_back(0);
            printf("aa2\n");
            param_dim = 0;
        }
        cout << "<PrimaryExp>" << endl;
    }
    else error();
}
void Number(){
    if(v_word[i].type=="INTCON"){
        cout << v_word[i].type << " " << v_word[i].name << endl;
        num_stack.push_back(v_word[i].name);
        i++;
        cout << "<Number>" << endl;
    }
    else error();
}
void UnaryExp(){
    if(v_word[i].name=="+"||v_word[i].name=="-"||v_word[i].name=="!"){
        int c=0;
        if(v_word[i].name=="-") c=1;
        else if(v_word[i].name=="!") c=2;
        UnaryOp();
        UnaryExp();
        if(c==1){
            x1 = num_stack.back();
            num_stack.pop_back();
            printf("0 - %s\n", x1.c_str());
            if(x1[0]=='%'&& check_num_type(x1)==1){
                code = "\t%"+to_string(register_no)+" = zext i1 "+x1+" to i32";
                mid_code.push_back(code);
                register_no++;
            }
            if(is_const_number==1||is_global==1){
                num_stack.push_back(to_string(-stoi(x1)));
            }
            else{
                code="\t%"+to_string(register_no)+" = sub i32 0, "+x1;
                mid_code.push_back(code);
                num_stack.push_back("%"+to_string(register_no));
                register_no++;
            }
        }
        if(c==2){
            x1 = num_stack.back();
            num_stack.pop_back();
            printf("0 == %s\n", x1.c_str());
            if(x1[0]=='%'&&check_num_type(x1)==1){
                code="\t%"+to_string(register_no)+" = icmp eq i1 "+x1+", 0";
            }
            else code="\t%"+to_string(register_no)+" = icmp eq i32 "+x1+", 0";
            mid_code.push_back(code);
            num_stack.push_back("%"+to_string(register_no));
            register_no++;
        }
    }
    else if(v_word[i].type=="IDENFR"&&v_word[i+1].name=="(") {
        vector<string> temp_num_stack;
        vector<string> temp_op_stack;
        temp_num_stack = num_stack;
        temp_op_stack = op_stack;
        num_stack.clear();
        op_stack.clear();
        vector<NUMBER> temp_params_stack = params_stack;
        for(int k1 = 0; k1 < params_stack.size(); k1++){
            printf("params_stack[%d] = %s\n", k1, params_stack[k1].num.c_str());
        }
        params_stack.clear();
        int i2 = i;
        cout << v_word[i].type << " " << v_word[i].name << endl;
        cout << v_word[i + 1].type << " " << v_word[i + 1].name << endl;
        i = i + 2;
        p = currentTable;
        int flag = 0;
        struct SYMBOL *symbol1= check_symbol(v_word[i2]);
        if (symbol1==NULL) {
            save_v_error('c', v_word[i2].line_number);
        }
        else if(symbol1->type!="voidFunc"&&symbol1->type!="intFunc"){
            save_v_error('c', v_word[i2].line_number);
        }
        else if(!stack.empty()&&symbol1->type=="voidFunc") {
            stack.back().push_back(-1);
        }
        else if(!stack.empty()&&symbol1->type=="intFunc") {
            stack.back().push_back(0);
        }
        if(IsExp(v_word[i])) {
            int i3=i-2;
            vector<int> g;
            stack.push_back(g);
            FuncRParams();
            check_func_params(v_word[i3], stack.back());
            stack.pop_back();
        }
        if (v_word[i].name == ")") {
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            string code1;
            if(symbol1->type=="intFunc"){
                code1 = "\t%"+ to_string(register_no)+" = call i32 @"+symbol1->name+"(";
                num_stack = temp_num_stack;
                op_stack = temp_op_stack;
                num_stack.push_back("%"+ to_string(register_no));
                register_no++;
            }
            else{
                code1 ="\tcall void @"+symbol1->name+"(";
            }
            for(int k1 =0; k1 < params_stack.size();k1++){
                printf("%s.dim = %d\n", params_stack[k1].num.c_str(), params_stack[k1].dim);
                if(params_stack[k1].dim==0){
                    code1 = code1 + "i32 "+params_stack[k1].num+", ";
                }
                else if(params_stack[k1].dim==1){
                    code1 = code1 + "i32* "+params_stack[k1].num+", ";
                }
                else if(params_stack[k1].dim == 2){
                    printf("number = %s aa number.dim1 = %d\n", params_stack[k1].num.c_str(), params_stack[k1].dim1);
                    code1 = code1 + "["+to_string(params_stack[k1].dim1)+" x i32]* "+params_stack[k1].num+", ";
                }
            }
            if(params_stack.size()!=0)
                code1 = code1.substr(0, code1.size()-2);
            code1 = code1 +")";
            mid_code.push_back(code1);
            params_stack = temp_params_stack;
            if(symbol1->type=="voidFunc") {
                printf("func_name = %s\n", symbol1->name.c_str());
                printf("aa3\n");
                param_dim=-1;
            }
            else if(symbol1->type=="intFunc") {
                printf("func_name = %s\n", symbol1->name.c_str());
                printf("aa4\n");
                param_dim=0;
            }
        } else save_v_error('j', v_word[i - 1].line_number);
    }
    else if(v_word[i].name=="("||v_word[i].type=="IDENFR"|| IsNumberString(v_word[i].name)){
        PrimaryExp();
    }
    else error();
    cout << "<UnaryExp>" << endl;
}
void UnaryOp(){
    if(v_word[i].name=="+"||v_word[i].name=="-"||v_word[i].name=="!"){
        cout << v_word[i].type << " " << v_word[i].name << endl;
        cout << "<UnaryOp>" << endl;
        i++;
    }
    else error();
}
void FuncRParams(){
    Exp();
    while(!op_stack.empty()){
        compute2();
    }
    number.dim1 = array_dim;
    number.dim=param_dim;
    param_dim = 0;
    array_dim = 0;
    number.num=num_stack.back();
    params_stack.push_back(number);
    num_stack.pop_back();
    while(v_word[i].name==","){
        cout << v_word[i].type << " " << v_word[i].name << endl;
        i++;
        Exp();
        while(!op_stack.empty()){
            compute2();
        }
        number.dim=param_dim;
        number.dim1 = array_dim;
        array_dim = 0;
        number.num=num_stack.back();
        params_stack.push_back(number);
        num_stack.pop_back();
    }
    cout << "<FuncRParams>" << endl;
}
void MulExp(){
    if(IsExp(v_word[i])){
        UnaryExp();
        while(v_word[i].name=="*"||v_word[i].name=="/"||v_word[i].name=="%"||v_word[i].name=="bitand"){
            string s = v_word[i].name;
            cout << "<MulExp>" << endl;
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            op_stack.push_back(s);
            UnaryExp();
            if(!op_stack.empty()&&op_stack.back()!="("){
                if(is_global==1||is_const_number==1) compute1();
                else compute2();
            }
        }

        cout << "<MulExp>" << endl;
    }
    else error();
}
void AddExp(){
    if(IsExp(v_word[i])){
        MulExp();
        while(v_word[i].name=="+"||v_word[i].name=="-"){
            string s = v_word[i].name;
            cout << "<AddExp>" << endl;
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            op_stack.push_back(s);
            MulExp();
            if(!op_stack.empty()&&op_stack.back()!="("){
                if(is_global==1||is_const_number==1) compute1();
                else compute2();
            }
        }

        cout << "<AddExp>" << endl;
    }
    else error();
}
void RelExp(){
    if(IsExp(v_word[i])){
        AddExp();
        while(v_word[i].name=="<"||v_word[i].name==">"||v_word[i].name=="<="||v_word[i].name==">="){
            string s = v_word[i].name;
            cout << "<RelExp>" << endl;
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            op_stack.push_back(s);
            AddExp();
            if(!op_stack.empty()&&op_stack.back()!="("){
                if(is_global==1) compute1();
                else compute2();
            }
        }

        cout << "<RelExp>" << endl;
    }
    else error();
}
void EqExp(){
    if(IsExp(v_word[i])){
        RelExp();
        while(v_word[i].name=="=="||v_word[i].name=="!="){
            string s = v_word[i].name;
            cout << "<EqExp>" << endl;
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            op_stack.push_back(s);
            RelExp();
            if(!op_stack.empty()&&op_stack.back()!="("){
                if(is_global==1) compute1();
                else compute2();
            }
        }

        cout << "<EqExp>" << endl;
    }
    else error();
}
void LAndExp(){
    if(IsExp(v_word[i])){
        EqExp();
        string s1 = num_stack.back();
        if(check_num_type(s1)!=1){
            num_stack.pop_back();
            code = "\t%"+to_string(register_no)+" = icmp ne i32 "+s1+", 0";
            mid_code.push_back(code);
            s1 = "%"+to_string(register_no);
            num_stack.push_back(s1);
            register_no++;
        }
        while(v_word[i].name=="&&"){
            cond_type=1;
            string s = v_word[i].name;
            cout << "<LAndExp>" << endl;
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            int temp = block_no+1,flag1=0;
            for(int i1 =i;v_word[i1].name!=")";i1++){
                if(v_word[i1].name=="("){
                    int j1=i1+1;
                    for(;v_word[j1].name!=")";j1++);
                    i1=j1;
                }
                if(v_word[i1].name=="&&"){
                    temp++;
                }
                if(v_word[i1].name=="||"){
                    flag1=1;
                    break;
                }
            }
            if(flag1==1){
                code = "\tbr i1 "+s1+", label %block"+to_string(block_no)+", label %block"+to_string(temp);
            }
            else{
                code = "\tbr i1 "+s1+", label %block"+to_string(block_no)+", label %block_out"+to_string(block1_no);
            }
            mid_code.push_back(code);
            code = "\nblock"+to_string(block_no)+":";
            block_no++;
            mid_code.push_back(code);
            num_stack.pop_back();
            EqExp();
            s1 = num_stack.back();
            if(check_num_type(s1)!=1){
                    num_stack.pop_back();
                    code = "\t%"+to_string(register_no)+" = icmp ne i32 "+s1+", 0";
                    mid_code.push_back(code);
                    s1 = "%"+to_string(register_no);
                    num_stack.push_back(s1);
                    register_no++;
                }
            }
        cout << "<LAndExp>" << endl;
    }
    else error();
}
void LOrExp(){
    if(IsExp(v_word[i])){
        LAndExp();
        string s1 = num_stack.back();
        if(check_num_type(s1)!=1){
            num_stack.pop_back();
            code = "\t%"+to_string(register_no)+" = icmp ne i32 "+s1+", 0";
            mid_code.push_back(code);
            s1 = "%"+to_string(register_no);
            num_stack.push_back(s1);
            register_no++;
        }
        while(v_word[i].name=="||"){
            cond_type=1;
            string s = v_word[i].name;
            cout << "<LOrExp>" << endl;
            cout << v_word[i].type << " " << v_word[i].name << endl;
            i++;
            code = "\tbr i1 "+s1+", label %block_exec"+ to_string(block1_no)+", label %block"+to_string(block_no);
            mid_code.push_back(code);
            code = "\nblock"+to_string(block_no)+":";
            block_no++;
            mid_code.push_back(code);
            num_stack.pop_back();
            LAndExp();
            s1 = num_stack.back();
            if(check_num_type(s1)!=1){
                num_stack.pop_back();
                code = "\t%"+to_string(register_no)+" = icmp ne i32 "+s1+", 0";
                mid_code.push_back(code);
                s1 = "%"+to_string(register_no);
                num_stack.push_back(s1);
                register_no++;
            }
        }
        cout << "<LOrExp>" << endl;
    }
    else error();
}
void ConstExp(){
    if(IsExp(v_word[i])){
        AddExp();
        while(!op_stack.empty()){
            compute1();
        }
        cout << "<ConstExp>" << endl;
    }
    else error();
}

bool IsNumber(char x)  {return (x >='0' && x <= '9');}  //判断是否为数字
bool IsAlpha(char x)  {return (x >= 'a' && x <= 'z')||(x >= 'A' && x <= 'Z');} //判断是否为字母
void lexical_analyzer(string data);  //词法分析
void save_v_word(string name, string type, int line_number){  //保存单词
    word.name=name;
    word.type=type;
    word.line_number=line_number;
    v_word.push_back(word);
} //保存单词
bool IsStmt(struct WORD w){
    return
            w.type=="IDENFR" ||
            w.name==";" ||
            IsExp(w) ||
            w.name=="{" ||
            w.name=="if" ||
            w.name=="while" ||
            w.name=="break" ||
            w.name=="continue" ||
            w.name=="return" ||
            w.name=="printf";
}
bool IsExp(struct WORD w){
    return
            w.name=="(" ||
            w.type=="IDENFR" ||
            IsNumberString(w.name) ||
            w.name=="+" ||
            w.name=="-" ||
            w.name=="!";
}

int check_num_type(string num){ //判断数字类型
    for(int i1=mid_code.size()-1; i1>=0; i1--){
        if(mid_code[i1].find(num+" = ") != string::npos&&num[0]=='%'){
            if(mid_code[i1].find(" icmp ") != string::npos
               ||mid_code[i1].find(" and i1") != string::npos
               ||mid_code[i1].find(" or ") != string::npos){
                return 1;
            }
            else return 32;
        }
    }
    return 0;
}
void compute2(){
    x1 = num_stack.back();
    num_stack.pop_back();
    x2 = num_stack.back();
    num_stack.pop_back();
    op = op_stack.back();
    op_stack.pop_back();
    printf("%s %s2 %s\n", x2.c_str(),op.c_str(), x1.c_str());
    if(op=="+"){
        if(check_num_type(x1)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x1+" to i32";
            x1 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        if(check_num_type(x2)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x2+" to i32";
            x2 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        code = "\t%"+to_string(register_no)+" = add i32 "+x2+", "+x1;
        mid_code.push_back(code);
        num_stack.push_back("%"+to_string(register_no));
        register_no++;
    }
    else if(op=="-"){
        if(check_num_type(x1)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x1+" to i32";
            x1 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        if(check_num_type(x2)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x2+" to i32";
            x2 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        code = "\t%"+to_string(register_no)+" = sub i32 "+x2+", "+x1;
        mid_code.push_back(code);
        num_stack.push_back("%"+to_string(register_no));
        register_no++;
    }
    else if(op=="*"){
        if(check_num_type(x1)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x1+" to i32";
            x1 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        if(check_num_type(x2)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x2+" to i32";
            x2 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        code = "\t%"+to_string(register_no)+" = mul i32 "+x2+", "+x1;
        mid_code.push_back(code);
        num_stack.push_back("%"+to_string(register_no));
        register_no++;
    }
    else if(op=="bitand"){
        if(check_num_type(x1)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x1+" to i32";
            x1 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        if(check_num_type(x2)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x2+" to i32";
            x2 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        code = "\t%"+to_string(register_no)+" = and i32 "+x2+", "+x1;
        mid_code.push_back(code);
        num_stack.push_back("%"+to_string(register_no));
        register_no++;
    }
    else if(op=="/"){
        if(check_num_type(x1)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x1+" to i32";
            x1 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        if(check_num_type(x2)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x2+" to i32";
            x2 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        code = "\t%"+to_string(register_no)+" = sdiv i32 "+x2+", "+x1;
        mid_code.push_back(code);
        num_stack.push_back("%"+to_string(register_no));
        register_no++;
    }
    else if(op=="%"){
        if(check_num_type(x1)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x1+" to i32";
            x1 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        if(check_num_type(x2)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x2+" to i32";
            x2 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        code = "\t%"+to_string(register_no)+" = srem i32 "+x2+", "+x1;
        mid_code.push_back(code);
        num_stack.push_back("%"+to_string(register_no));
        register_no++;
    }
    else if(op=="<"){
        if(check_num_type(x1)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x1+" to i32";
            x1 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        if(check_num_type(x2)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x2+" to i32";
            x2 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        code = "\t%"+to_string(register_no)+" = icmp slt i32 "+x2+", "+x1;
        mid_code.push_back(code);
        num_stack.push_back("%"+to_string(register_no));
        register_no++;
    }
    else if(op==">"){
        if(check_num_type(x1)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x1+" to i32";
            x1 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        if(check_num_type(x2)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x2+" to i32";
            x2 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        code = "\t%"+to_string(register_no)+" = icmp sgt i32 "+x2+", "+x1;
        mid_code.push_back(code);
        num_stack.push_back("%"+to_string(register_no));
        register_no++;
    }
    else if(op=="<="){
        if(check_num_type(x1)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x1+" to i32";
            x1 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        if(check_num_type(x2)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x2+" to i32";
            x2 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        code = "\t%"+to_string(register_no)+" = icmp sle i32 "+x2+", "+x1;
        mid_code.push_back(code);
        num_stack.push_back("%"+to_string(register_no));
        register_no++;
    }
    else if(op==">="){
        if(check_num_type(x1)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x1+" to i32";
            x1 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        if(check_num_type(x2)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x2+" to i32";
            x2 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        code = "\t%"+to_string(register_no)+" = icmp sge i32 "+x2+", "+x1;
        mid_code.push_back(code);
        num_stack.push_back("%"+to_string(register_no));
        register_no++;
    }
    else if(op=="=="){
        if(check_num_type(x1)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x1+" to i32";
            x1 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        if(check_num_type(x2)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x2+" to i32";
            x2 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        code = "\t%"+to_string(register_no)+" = icmp eq i32 "+x2+", "+x1;
        mid_code.push_back(code);
        num_stack.push_back("%"+to_string(register_no));
        register_no++;
    }
    else if(op=="!=") {
        if(check_num_type(x1)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x1+" to i32";
            x1 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        if(check_num_type(x2)==1){
            code = "\t%"+to_string(register_no)+" = zext i1 "+x2+" to i32";
            x2 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        code = "\t%"+to_string(register_no)+" = icmp ne i32 "+x2+", "+x1;
        mid_code.push_back(code);
        num_stack.push_back("%"+to_string(register_no));
        register_no++;
    }
    else if(op=="&&"){
        if(check_num_type(x1)!=1){
            code = "\t%"+ to_string(register_no)+" = icmp ne i32 "+x1+", 0";
            x1 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        if(check_num_type(x2)!=1){
            code = "\t%"+ to_string(register_no)+" = icmp ne i32 "+x2+", 0";
            x2 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        code = "\t%"+to_string(register_no)+" = and i1 "+x2+", "+x1;
        mid_code.push_back(code);
        num_stack.push_back("%"+to_string(register_no));
        register_no++;
    }
    else if(op=="||"){
        if(check_num_type(x1)!=1){
            code = "\t%"+ to_string(register_no)+" = icmp ne i32 "+x1+", 0";
            x1 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        if(check_num_type(x2)!=1){
            code = "\t%"+ to_string(register_no)+" = icmp ne i32 "+x2+", 0";
            x2 = "%"+to_string(register_no);
            register_no++;
            mid_code.push_back(code);
        }
        code = "\t%"+to_string(register_no)+" = or i1 "+x2+", "+x1;
        mid_code.push_back(code);
        num_stack.push_back("%"+to_string(register_no));
        register_no++;
    }
}
void compute1() {
    string temp;
    x1 = num_stack.back();
    num_stack.pop_back();
    x2 = num_stack.back();
    num_stack.pop_back();
    op = op_stack.back();
    op_stack.pop_back();
    printf("%s %s1 %s\n", x2.c_str(),op.c_str(), x1.c_str());
    if(op=="+"){
        temp = to_string(stoi(x1)+stoi(x2));
        num_stack.push_back(temp);
    }
    else if(op=="-"){
        temp = to_string(stoi(x2)-stoi(x1));
        num_stack.push_back(temp);
    }
    else if(op=="*"){
        temp = to_string(stoi(x2)*stoi(x1));
        num_stack.push_back(temp);
    }
    else if(op=="bitand"){
        temp = to_string(stoi(x2)&stoi(x1));
        num_stack.push_back(temp);
    }
    else if(op=="/"){
        temp = to_string(stoi(x2)/stoi(x1));
        num_stack.push_back(temp);
    }
    else if(op=="%"){
        temp = to_string(stoi(x2)%stoi(x1));
        num_stack.push_back(temp);
    }
}
void save_array_value(string name, int type, vector<int> upper, string value[MAXN][MAXN],int flag1){ //save array value to global variable
    p=currentTable;
    struct SYMBOL *s = NULL;
    while(p!=NULL){
        if(p->symbol.count(name)!=0){
            s=&p->symbol[name];
            break;
        }
        p=p->parent;
    }
    if(upper.size()==1){
        if(type==1){
            code = "@"+name+" = constant ["+to_string(upper[0])+ " x i32] ";
        }
        else{
            code = "@"+name+" = global ["+to_string(upper[0])+ " x i32] ";
        }
        if(flag1==1){
            code = code +"[";
            for(int i1=0; i1<upper[0]; i1++){
                code =code + "i32 "+value[0][i1]+", ";
                if(type==1)
                    s->arr[0][i1]=value[0][i1];
            }
            code = code.substr(0,code.length()-2);
            code = code + "]";
        }
        else {
            code = code + "zeroinitializer";
            for(int i1=0; i1<upper[0]; i1++){
                if(type==1)
                    s->arr[0][i1]="";
            }
        }
        mid_code.push_back(code);
    }
    else if(upper.size()==2){
        if(type==1){
            code = "@"+name+" = constant ["+to_string(upper[0])+ " x ["+ to_string(upper[1])+" x i32]] ";
            for(int i1=0;i1<upper[0];i1++){
                for(int i2=0;i2<upper[1];i2++){
                    s->arr[i1][i2]=value[i1][i2];
                }
            }
        }
        else
            code = "@"+name+" = global ["+to_string(upper[0])+ " x ["+ to_string(upper[1])+" x i32]] ";
        if(flag1==1) {
            code = code + "[";
            for (int i1 = 0; i1 < upper[0]; i1++) {
                code = code + "[" + to_string(upper[1]) + " x i32] [";
                for (int i2 = 0; i2 < upper[1]; i2++) {
                    code = code + "i32 " + value[i1][i2] + ", ";
                }
                code = code.substr(0, code.length() - 2);
                code = code + "], ";
            }
            code = code.substr(0, code.length() - 2);
            code = code + "]";
        }
        else code = code + "zeroinitializer";
        mid_code.push_back(code);
    }
    else if(upper.size()==0){
        if(type==1){
            if(flag1==1){
                code = "@"+name+" = constant i32 "+value[0][0];
                s->value=value[0][0];
            }
            else{
                code = "@"+name+" = constant i32 0";
                s->value="0";
            }
            s->address = "@"+name;
        }
        else{
            if(flag1==1){
                code = "@"+name+" = global i32 "+value[0][0];
                s->value=value[0][0];
            }
            else{
                code = "@"+name+" = global i32 0";
                s->value="0";
            }
            s->address = "@"+name;
        }
        mid_code.push_back(code);
    }
}
void save_value1(string name,string address, vector<int> upper, string value[MAXN][MAXN]){ //save array value
    p=currentTable;
    struct SYMBOL *s = NULL;
    while(p!=NULL){
        if(p->symbol.count(name)!=0){
            s=&p->symbol[name];
            break;
        }
        p=p->parent;
    }
    if(upper.size()==1) {
        for(int i1 = 0;i1<upper[0];i1++){
            s->arr[0][i1]=value[0][i1];
        }
        code = "\t%"+ to_string(register_no)+" = getelementptr ["+to_string(upper[0])+" x i32], ["+to_string(upper[0])+" x i32]* "+address+", i32 0, i32 0";
        mid_code.push_back(code);
        string base = "%"+to_string(register_no);
        register_no++;
        for(int i1=0; i1<upper[0]; i1++){
            code = "\t%"+to_string(register_no)+" = getelementptr i32, i32* "+base+", i32 "+to_string(i1);
            mid_code.push_back(code);
            register_no++;
            code = "\tstore i32 "+value[0][i1]+", i32* %"+to_string(register_no-1);
            mid_code.push_back(code);
        }
    }
    else if(upper.size()==2){
        for(int i1 = 0; i1<upper[0]; i1++){
            for(int i2 = 0; i2<upper[1]; i2++){
                s->arr[i1][i2]=value[i1][i2];
            }
        }
        code = "\t%"+ to_string(register_no)+" = getelementptr ["+to_string(upper[0])+" x ["+to_string(upper[1])+" x i32]], ["+to_string(upper[0])+" x ["+to_string(upper[1])+" x i32]]* "+address+", i32 0, i32 0";
        mid_code.push_back(code);
        register_no++;
        code = "\t%"+ to_string(register_no)+" = getelementptr ["+to_string(upper[1])+" x i32], ["+to_string(upper[1])+" x i32]* %"+to_string(register_no-1)+", i32 0, i32 0";
        mid_code.push_back(code);
        string base = "%"+to_string(register_no);
        register_no++;
        int size = upper[0] * upper[1] * 4;
        code = "\tcall void @memset(i32* %"+to_string(register_no-1)+", i32 0, i32 "+to_string(size)+")";
        mid_code.push_back(code);
        for(int i1 = 0; i1<upper[0]; i1++){
            for(int i2 = 0; i2<upper[1]; i2++){
                if(value[i1][i2]!="0"){
                    code = "\t%"+to_string(register_no)+" = getelementptr i32, i32* "+base+", i32 "+to_string(i1*upper[1]+i2);
                    register_no++;
                    mid_code.push_back(code);
                    code = "\tstore i32 "+value[i1][i2]+", i32* %"+ to_string(register_no-1);
                    mid_code.push_back(code);
                }
            }
        }
    }
    else if(upper.size()==0){
        s->value = value[0][0];
    }
}
void save_value2(string name,string address, vector<int> upper, string value[MAXN][MAXN], int flag){ //value[0][0] = "a"
    p=currentTable;
    struct SYMBOL *s = NULL;
    while(p!=NULL){
        if(p->symbol.count(name)!=0){
            s=&p->symbol[name];
            break;
        }
        p=p->parent;
    }
    if(upper.size()==1){
        code = "\t%"+ to_string(register_no)+" = getelementptr ["+to_string(upper[0])+" x i32], ["+to_string(upper[0])+" x i32]* "+address+", i32 0, i32 0";
        mid_code.push_back(code);
        string base = "%"+to_string(register_no);
        register_no++;
        int size = upper[0] * 4;
        code = "\tcall void @memset(i32* %"+to_string(register_no-1)+", i32 0, i32 "+to_string(size)+")";
        mid_code.push_back(code);
        for(int i1 = 0; i1<upper[0]; i1++){
            if(flag==1&&value[0][i1]!="0"){
                code = "\t%"+ to_string(register_no)+" = getelementptr i32, i32* "+base+", i32 "+to_string(i1);
                register_no++;
                mid_code.push_back(code);
                code = "\tstore i32 "+value[0][i1]+", i32* %"+to_string(register_no-1);
                mid_code.push_back(code);
            }
        }
    }
    else if(upper.size()==2){
        code = "\t%"+ to_string(register_no)+" = getelementptr ["+to_string(upper[0])+" x ["+to_string(upper[1])+" x i32]], ["+to_string(upper[0])+" x ["+to_string(upper[1])+" x i32]]* "+address+", i32 0, i32 0";
        mid_code.push_back(code);
        register_no++;
        code = "\t%"+ to_string(register_no)+" = getelementptr ["+to_string(upper[1])+" x i32], ["+to_string(upper[1])+" x i32]* %"+to_string(register_no-1)+", i32 0, i32 0";
        mid_code.push_back(code);
        string base = "%"+to_string(register_no);
        register_no++;
        int size = upper[0] * upper[1] * 4;
        code = "\tcall void @memset(i32* %"+to_string(register_no-1)+", i32 0, i32 "+to_string(size)+")";
        mid_code.push_back(code);
        for(int i1 = 0; i1<upper[0]; i1++){
            for(int i2 = 0; i2<upper[1]; i2++){
                if(flag==1&&value[i1][i2]!="0"){
                    code = "\t%"+ to_string(register_no)+" = getelementptr i32, i32* "+base+", i32 "+to_string(i1*upper[1]+i2);
                    register_no++;
                    mid_code.push_back(code);
                    code = "\tstore i32 "+value[i1][i2]+", i32* %"+ to_string(register_no-1);
                    mid_code.push_back(code);
                }
            }
        }
    }
    else if(upper.size()==0){
        s->value = value[0][0];
    }
}
string get_array(string name, vector<string> upper,int flag){
    p=currentTable;
    struct SYMBOL *s = NULL;
    while(p!=NULL){
        if(p->symbol.count(name)!=0){
            s=&p->symbol[name];
            break;
        }
        p=p->parent;
    }
    if(s!=NULL){
        if(s->type!="ParamNumber"){
            if(s->dim==2){
                if(upper.size()==2) {
                    if(upper[0][0]!='%'&&upper[1][0]!='%'&&flag==0){
                        if(s->arr[stoi(upper[0])][stoi(upper[1])]!="")
                            return s->arr[stoi(upper[0])][stoi(upper[1])];
                    }
                    if(upper[0][0]=='%')
                        code = "\t%" + to_string(register_no) + " = getelementptr [" + to_string(s->dim1) + " x [" +
                               to_string(s->dim2) + " x i32]], [" + to_string(s->dim1) + " x [" + to_string(s->dim2) +
                               " x i32]]* " + s->address + ", i64 0, i32 "+ upper[0];
                    else
                        code = "\t%" + to_string(register_no) + " = getelementptr [" + to_string(s->dim1) + " x [" +
                               to_string(s->dim2) + " x i32]], [" + to_string(s->dim1) + " x [" + to_string(s->dim2) +
                               " x i32]]* " + s->address + ", i64 0, i64 " + upper[0];
                    mid_code.push_back(code);
                    register_no++;
                    if(upper[1][0]=='%')
                        code = "\t%" + to_string(register_no) + " = getelementptr [" + to_string(s->dim2) + " x i32], [" +
                               to_string(s->dim2) + " x i32]* %" + to_string(register_no - 1) + ", i64 0, i32 " + upper[1];
                    else
                        code = "\t%" + to_string(register_no) + " = getelementptr [" + to_string(s->dim2) + " x i32], [" +
                               to_string(s->dim2) + " x i32]* %" + to_string(register_no - 1) + ", i64 0, i64 " + upper[1];
                    mid_code.push_back(code);
                    register_no++;
                    if(flag==0){
                        code = "\t%" + to_string(register_no) + " = load i32, i32* %" + to_string(register_no - 1);
                        mid_code.push_back(code);
                        register_no++;
                    }
                }
                else if(upper.size()==1){
                    if(upper[0][0]=='%')
                        code = "\t%" + to_string(register_no) + " = getelementptr [" + to_string(s->dim1) + " x [" +
                               to_string(s->dim2) + " x i32]], [" + to_string(s->dim1) + " x [" + to_string(s->dim2) +
                               " x i32]]* " + s->address + ", i64 0, i32 "+ upper[0];
                    else
                        code = "\t%" + to_string(register_no) + " = getelementptr [" + to_string(s->dim1) + " x [" +
                               to_string(s->dim2) + " x i32]], [" + to_string(s->dim1) + " x [" + to_string(s->dim2) +
                               " x i32]]* " + s->address + ", i64 0, i64 " + upper[0];
                    mid_code.push_back(code);
                    register_no++;
                    code = "\t%" + to_string(register_no) + " = getelementptr [" + to_string(s->dim2) + " x i32], [" +
                           to_string(s->dim2) + " x i32]* %" + to_string(register_no - 1) + ", i32 0, i32 0";
                    mid_code.push_back(code);
                    register_no++;
                }
                else if(upper.size()==0){
                    code = "\t%" + to_string(register_no) + " = getelementptr [" + to_string(s->dim1) + " x [" +
                           to_string(s->dim2) + " x i32]], [" + to_string(s->dim1) + " x [" + to_string(s->dim2) +
                           " x i32]]* " + s->address + ", i32 0, i32 0";
                    mid_code.push_back(code);
                    register_no++;

                }
                return "%"+to_string(register_no-1);
            }
            else if(s->dim==1){
                if(upper.size()==0){
                    code = "\t%"+ to_string(register_no)+" = getelementptr ["+to_string(s->dim1)+" x i32], ["+
                           to_string(s->dim1)+" x i32]* "+s->address+", i32 0, i32 0";
                    mid_code.push_back(code);
                    register_no++;
                }
                else if(upper.size()==1){
                    if(upper[0][0]!='%'&&flag==0){
                        printf("s->arr[0][stoi(upper[0])]=aa%saa\n",s->arr[0][stoi(upper[0])].c_str());
                        if(s->arr[0][stoi(upper[0])]!="")
                            return s->arr[0][stoi(upper[0])];
                    }
                    if(upper[0][0]=='%')
                        code = "\t%"+ to_string(register_no)+" = getelementptr ["+to_string(s->dim1)+" x i32], ["+
                               to_string(s->dim1)+" x i32]* "+s->address+", i32 0, i32 "+upper[0];
                    else
                        code = "\t%"+ to_string(register_no)+" = getelementptr ["+to_string(s->dim1)+" x i32], ["+
                               to_string(s->dim1)+" x i32]* "+s->address+", i32 0, i64 "+upper[0];
                    mid_code.push_back(code);
                    register_no++;
                    if(flag==0){
                        code = "\t%"+ to_string(register_no)+" = load i32, i32* %"+to_string(register_no-1);
                        mid_code.push_back(code);
                        register_no++;
                    }
                }
                return "%"+to_string(register_no-1);
            }
        }
        else{
            if(s->dim==2){
                code = "\t%"+to_string(register_no)+" = load ["+to_string(s->dim2)+" x i32]*, ["+
                       to_string(s->dim2)+" x i32]** "+s->address;
                mid_code.push_back(code);
                register_no++;
                if(upper.size()==2){
                    if(upper[0][0]=='%')
                        code = "\t%"+to_string(register_no)+" = getelementptr ["+to_string(s->dim2)+" x i32], ["+
                               to_string(s->dim2)+" x i32]* %"+to_string(register_no-1)+", i32 "+upper[0];
                    else
                        code = "\t%"+to_string(register_no)+" = getelementptr ["+to_string(s->dim2)+" x i32], ["+
                               to_string(s->dim2)+" x i32]* %"+to_string(register_no-1)+", i64 "+upper[0];
                    mid_code.push_back(code);
                    register_no++;
                    if(upper[1][0]=='%')
                        code = "\t%"+ to_string(register_no)+" = getelementptr ["+to_string(s->dim2)+" x i32], ["+
                               to_string(s->dim2)+" x i32]* %"+to_string(register_no-1)+", i32 0, i32 "+upper[1];
                    else
                        code = "\t%"+ to_string(register_no)+" = getelementptr ["+to_string(s->dim2)+" x i32], ["+
                               to_string(s->dim2)+" x i32]* %"+to_string(register_no-1)+", i64 0, i64 "+upper[1];
                    mid_code.push_back(code);
                    register_no++;
                    if(flag==0){
                        code = "\t%" + to_string(register_no) + " = load i32, i32* %" + to_string(register_no - 1);
                        mid_code.push_back(code);
                        register_no++;
                    }
                }
                else if(upper.size()==1){
                    if(upper[0][0]=='%')
                        code = "\t%"+to_string(register_no)+" = getelementptr ["+to_string(s->dim2)+" x i32], ["+
                               to_string(s->dim2)+" x i32]* %"+to_string(register_no-1)+", i32 "+upper[0];
                    else
                        code = "\t%"+to_string(register_no)+" = getelementptr ["+to_string(s->dim2)+" x i32], ["+
                               to_string(s->dim2)+" x i32]* %"+to_string(register_no-1)+", i64 "+upper[0];
                    mid_code.push_back(code);
                    register_no++;
                    code = "\t%"+ to_string(register_no)+" = getelementptr ["+to_string(s->dim2)+" x i32], ["+
                           to_string(s->dim2)+" x i32]* %"+to_string(register_no-1)+", i32 0, i32 0";
                    mid_code.push_back(code);
                    register_no++;
                }
                return "%"+to_string(register_no-1);
            }
            else if(s->dim==1){
                code = "\t%"+ to_string(register_no)+" = load i32*, i32** "+s->address;
                mid_code.push_back(code);
                register_no++;
                if(upper.size()==1){//%5 = getelementptr inbounds i32, i32* %4, i64 2
                    if(upper[0][0]=='%')
                        code = "\t%"+ to_string(register_no)+" = getelementptr i32, i32* %"+ to_string(register_no-1)+", i32 "+upper[0];
                    else
                        code = "\t%"+ to_string(register_no)+" = getelementptr i32, i32* %"+ to_string(register_no-1)+", i64 "+upper[0];
                    mid_code.push_back(code);
                    register_no++;
                    if(flag==0){
                        code = "\t%" + to_string(register_no) + " = load i32, i32* %" + to_string(register_no - 1);
                        mid_code.push_back(code);
                        register_no++;
                    }
                }
                return "%"+to_string(register_no-1);
            }
        }

    }
    return NULL;
}
void backFill_func_param(string name, int func_param_dim, vector<string> c){ //c is the value of the parameter
    for(int i1 = mid_code.size()-1; i1 >= 0; i1--){
        if(mid_code[i1].find("define") != string::npos
           &&mid_code[i1].find("@"+name) != string::npos){
            if(func_param_dim==0){
                mid_code[i1] =mid_code[i1]+"i32, ";
            }
            else if(func_param_dim==1){
                mid_code[i1] =mid_code[i1]+"i32*, ";
            }
            else if(func_param_dim==2){
                mid_code[i1] = mid_code[i1]+"["+c[0]+" x i32]*, ";
            }
            break;
        }
    }
}
void backFill_func_end(string name){
    for(int i1 = mid_code.size()-1; i1 >= 0; i1--){
        if(mid_code[i1].find("define") != string::npos
           &&mid_code[i1].find("@"+name) != string::npos){
            if(mid_code[i1][mid_code[i1].size()-2]==','){
                mid_code[i1].erase(mid_code[i1].size()-2,2);
            }
            mid_code[i1] =mid_code[i1]+") {";
            break;
        }
    }
}
bool IsNumberString(string x){ //判断字符串是否为数字
    int len = x.length();
    int flag=0;
    for(int s=0; s<len; s++){
        if(!IsNumber(x[s])){
            flag=1;
            break;
        }
    }
    if(flag==1){
        return false;
    }
    else return true;
}

void error(){ //错误处理
    printf("11%d %s %d %s\n", i,v_word[i].name.c_str(), v_word[i].line_number,v_word[i-1].name.c_str());
}
void save_v_error(char type, int line_number){ //保存错误信息
    e.type=type;
    e.line_number=line_number;
    v_error.push_back(e);
}
struct SYMBOL* check_symbol(struct WORD w){ //检查符号表中是否存在该符号
    p=currentTable;
    while(p!=NULL){
        if(p->symbol.count(w.name)!=0){
            return &p->symbol[w.name];
        }
        p=p->parent;
    }
    return NULL;
}
void save_symbol(struct SYMBOL s){ //保存符号信息
    p = currentTable;
    p->symbol[s.name] = s;
}
void SymbolInit(struct SYMBOL s){ //符号初始化
    s.name = "\0";
    s.type = "\0";
    s.dim = 0;
    s.func_params.clear();
}
void check_func_params(struct WORD w, vector<int> z){ //检查函数参数是否匹配
    struct SYMBOL *symbol1= check_symbol(w);
    if(symbol1!=NULL){
        if(symbol1->func_params.size()!=z.size()){
            save_v_error('d', w.line_number);
        }
        else{
            for(int u=0;u<z.size();u++){
                if(symbol1->func_params[u]!=z[u]){
                    save_v_error('e', w.line_number);
                    break;
                }
            }
        }
    }
}