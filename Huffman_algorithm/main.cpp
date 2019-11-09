#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include "Huffman.h"

using namespace std;

struct Node
{
    unsigned char Key;//Сивол.
    Node* Left;
    Node* Right;
    string code;//Код символа
    explicit Node( unsigned char key ): Key( key ), Left( 0 ), Right( 0 ){}
};

class HuffmanTree
{
public:

    //Конструктор.
    HuffmanTree(): root( 0 ){}
    //Деструктор.
    ~HuffmanTree();

    Node* GiveRoot();
    void MakeTree( string& data, IInputStream& input );
    void MakeCodeForTree();
    vector<Node*> TreeEncode( IOutputStream& output );
    void BuildFromCode( IInputStream& input );

private:
    Node* root;

    void DeleteTree( Node* currentNode );
    void MakeCode( Node* currentNode, string prefix );
};

HuffmanTree::~HuffmanTree()
{
    DeleteTree( root );
}

//Удаление поддерева с корнем currentNode.
void HuffmanTree::DeleteTree( Node* currentNode )
{
    if( currentNode == 0 ){
        return;
    }
    DeleteTree( currentNode->Left );
    DeleteTree( currentNode->Right );
    delete currentNode;
}

//Возвращает указатель на корень дерева.
Node* HuffmanTree::GiveRoot()
{
    return root;
}

//Создаёт дерево для алгоритма Хаффмана. Возвращает указатель на корень.
void HuffmanTree::MakeTree( string& data, IInputStream& input )
{
    vector<Node*> HuffmanArr;
	vector<int> countSimbols;
	unsigned char value = 1;
	byte value1;

	while( input.Read( value1 ) ){
		value = static_cast<unsigned char>( value1 );
		data.push_back( value );
        int i = 0;

        for(; i < HuffmanArr.size(); ++i ){
            if( HuffmanArr[i]->Key == value ){
                ++countSimbols[i];
                break;
            }
        }

        if( i == HuffmanArr.size() ){
			Node* Node1 = new Node( value );
            HuffmanArr.push_back( Node1 );
            countSimbols.push_back( 1 );
        }
    }

    int min1;
    int min2;

    while( HuffmanArr.size() != 1 ){
        min1 = 0;

        for( int j = 1; j < HuffmanArr.size(); ++j ){
            if( countSimbols[j] < countSimbols[min1] ){
                min1 = j;
            }
        }

        if(min1 == 0){
            min2 = 1;
        }else{
            min2 = 0;
        }

        for( int j = 1; j < HuffmanArr.size(); ++j ){
            if( countSimbols[j] < countSimbols[min2] && j != min1 ){
                min2 = j;
            }
        }

        Node* NewNode = new Node( 0 );
        NewNode->Left = HuffmanArr[min1];
        NewNode->Right = HuffmanArr[min2];
        countSimbols[min1] = countSimbols[min1] + countSimbols[min2];
        HuffmanArr[min1] = NewNode;
        swap( HuffmanArr[min2], HuffmanArr[HuffmanArr.size() - 1] );
        HuffmanArr.pop_back();
        swap( countSimbols[min2], countSimbols[countSimbols.size() - 1] );
        countSimbols.pop_back();
    }

    root = HuffmanArr[0];
}

//Составление кода для каждого символа.
void HuffmanTree::MakeCodeForTree()
{
    if( root->Left == 0 ){
		root->code.push_back( '0' );
		return;
	}
    string prefix;
	prefix.push_back( '0' );
    MakeCode( root->Left, prefix );
    prefix[0] = '1';
    MakeCode( root->Right, prefix );
}

//Составление кода для элементов поддерева с корнем currentNode.
void HuffmanTree::MakeCode( Node* currentNode, string prefix )
{
    if( currentNode->Left == 0 ){
		currentNode->code = prefix;
		return;
    }
    prefix.push_back( '0' );
	MakeCode( currentNode->Left, prefix);
    prefix[prefix.size() - 1] += 1;
    MakeCode( currentNode->Right, prefix );
}

//Выводит код дерева Хаффмана.
//Возвращает вектор с листьями дерева, необходимый для упрощения дальнейшей работы программы.
vector<Node*> HuffmanTree::TreeEncode( IOutputStream& output )
{
    queue<Node*> q;
    vector<Node*> simbols(256);
    vector<Node*> simbolsInOrder;
    unsigned char c = 0;//Здесь будет формироваться байт для записи.
    q.push( root );
    short counter = 0;

    //Обход дерева в ширину.
    while( !q.empty() ){
		if( q.front()->Left != 0 ){
			q.push( q.front()->Left );
			q.push( q.front()->Right );
            c = ( c << 1 );//Если узел не листовой, то дописываем в с 0.
		}else{
			c = ( c << 1 ) + 1;//Если узел листовой, то дописываем в с 1.
            simbols[static_cast<int>( q.front()->Key )] = q.front();
            simbolsInOrder.push_back( q.front() );
		}
		q.pop();
		counter++;
		//Если с заполнен.
		if( counter == 8 ){
			output.Write( static_cast<byte>( c ) );//Выводим с.
            c = 0;//Обнуляем с.
            counter = 0;//Обнуляем счётчик.
        }
	}

	//Выводим "остаток". Биты, несущие информацию стоят в начале байта.
	if( counter != 0 ){
		c = c << ( 8 - counter );
		output.Write( static_cast<byte>( c ) );
	}
	//Выводим значения ключей в листьях дерева в порядке обхода.
	for( int i = 0; i < simbolsInOrder.size(); ++i ){
		output.Write( static_cast<byte>( simbolsInOrder[i]->Key ) );
	}
	return simbols;
}

//Построение дерева на основе считываемого кода.
void HuffmanTree::BuildFromCode( IInputStream& input )
{
    queue<Node*> q;
    vector<Node*> simbols;
    unsigned char c;//Здесь будет храниться считанный байт.
    root = new Node( 0 );
    q.push( root );
    byte value;

	while( !q.empty() ){
		input.Read( value );
		c = static_cast<unsigned char>( value );
		for( int i = 0; i < 8; ++i ){
			if( c >> 7 == 0 ){
				q.front()->Left = new Node( 0 );
				q.front()->Right = new Node( 0 );
                q.push( q.front()->Left );
                q.push( q.front()->Right );
                q.pop();
            }else{
                simbols.push_back( q.front() );
                q.pop();
			}
			if( q.empty() ){
				break;
			}
			c = c << 1;
		}
    }
    for( int i = 0; i < simbols.size(); ++i ){
        input.Read( value );
        simbols[i]->Key = static_cast<char>( value );
    }
}

//Кодирует и выводит входные данные.
void EncodeText( HuffmanTree& Tree, string& data, vector<Node*> simbols, IOutputStream& output )
{
	unsigned char c = 0;
    string s;
    short counter = 0;
	int sumOfCodes = 0;

	//Рассчитываем длину кода, который получится при кодировке данных.
	for( int i = 0; i < data.size(); ++i ){
		sumOfCodes += simbols[static_cast<unsigned int>( static_cast<unsigned char>( data[i] ) )]->code.size();
    }

    //Находим, сколько бит нехватает коду до целого числа байтов.
	short remain = 8 - ( sumOfCodes % 8 );

        //Ставим в начало кода необходимое количество нулей и единицу.
        for( int i = 0; i < remain - 1; ++i ){
            ++counter;
        }

        c = ( c << 1 ) + 1;
		++counter;
		//Если в коде целое число байтов, то придётся вывести в начале байт 00000001.
		if( counter == 8 ){
				output.Write( static_cast<byte>( c ) );
                c = 0;
				counter = 0;
        }

    for( int i = 0; i < data.size(); ++i ){
        s = simbols[static_cast<unsigned int>( static_cast<unsigned char>( data[i] ) )]->code;
        for( int j = 0; j < s.size(); ++j ){
            if( s[j] == '0' ){
                c = c << 1;
            }else{
                c = ( c << 1 ) + 1;
            }
            ++counter;
            if( counter == 8 ){
                output.Write( static_cast<byte>( c ) );
                c = 0;
                counter = 0;
            }
        }
    }

    if( counter != 0 ){
		c =  c << ( 8 - counter );
		output.Write( static_cast<byte>( c ) );
    }
}

//Декодирует текст.
void DecodeText( HuffmanTree& Tree, IInputStream& input, IOutputStream& output )
{
    unsigned char c;
    Node* currentNode = Tree.GiveRoot();
    int i = 0;
    bool b = false;
    byte value;

	//Декодировка текста.
	while( input.Read( value ) ){
        c = static_cast<unsigned char>( value );
        //Если считывание только началось, то в с сначала идёт дополнение данных до целого числа байтов.
        if( !b ){

            while( c >> 7 == 0 ){
                    c = c << 1;
                    ++i;
            }

            c = c << 1;
			++i;
			//Если только что был полностью разобран считанный байт, то необходимо снова считать элемент.
			if( i == 8 ){
				i = 0;
				input.Read( value );
				c = static_cast<unsigned char>( value );
			}
			b = true;
        }

        while( true ){
            //Если дошли до листа.
            if( currentNode->Left == 0 ){
                output.Write( static_cast<byte>( currentNode->Key ) );
                currentNode = Tree.GiveRoot();
            }else{
                //Если полностью разобран считанный байт.
                if( i == 8 ){
                    i = 0;
                    break;
                }
                if( c >> 7 == 0 ){
                    currentNode = currentNode->Left;//Ищем ключ в левом поддереве.
                }else{
                    currentNode = currentNode->Right;//Ищем ключ в правом поддереве.
                }
                c = c << 1;
                ++i;
            }
        }
    }
}

void Encode( IInputStream& original, IOutputStream& compressed )
{
    string data;
    HuffmanTree Tree;
	Tree.MakeTree( data, original );
	Tree.MakeCodeForTree();
	EncodeText( Tree, data, Tree.TreeEncode( compressed ), compressed );
}

void Decode( IInputStream& compressed, IOutputStream& original )
{
    HuffmanTree Tree;
    Tree.BuildFromCode( compressed );
	DecodeText( Tree, compressed, original );
}
