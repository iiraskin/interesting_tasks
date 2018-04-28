#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include "Huffman.h"

using namespace std;

struct Node
{
    unsigned char Key;//�����.
    Node* Left;
    Node* Right;
    string code;//��� �������
    explicit Node( unsigned char key ): Key( key ), Left( 0 ), Right( 0 ){}
};

class HuffmanTree
{
public:

    //�����������.
    HuffmanTree(): root( 0 ){}
    //����������.
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

//�������� ��������� � ������ currentNode.
void HuffmanTree::DeleteTree( Node* currentNode )
{
    if( currentNode == 0 ){
        return;
    }
    DeleteTree( currentNode->Left );
    DeleteTree( currentNode->Right );
    delete currentNode;
}

//���������� ��������� �� ������ ������.
Node* HuffmanTree::GiveRoot()
{
    return root;
}

//������ ������ ��� ��������� ��������. ���������� ��������� �� ������.
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

//����������� ���� ��� ������� �������.
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

//����������� ���� ��� ��������� ��������� � ������ currentNode.
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

//������� ��� ������ ��������.
//���������� ������ � �������� ������, ����������� ��� ��������� ���������� ������ ���������.
vector<Node*> HuffmanTree::TreeEncode( IOutputStream& output )
{
    queue<Node*> q;
    vector<Node*> simbols(256);
    vector<Node*> simbolsInOrder;
    unsigned char c = 0;//����� ����� ������������� ���� ��� ������.
    q.push( root );
    short counter = 0;

    //����� ������ � ������.
    while( !q.empty() ){
		if( q.front()->Left != 0 ){
			q.push( q.front()->Left );
			q.push( q.front()->Right );
            c = ( c << 1 );//���� ���� �� ��������, �� ���������� � � 0.
		}else{
			c = ( c << 1 ) + 1;//���� ���� ��������, �� ���������� � � 1.
            simbols[static_cast<int>( q.front()->Key )] = q.front();
            simbolsInOrder.push_back( q.front() );
		}
		q.pop();
		counter++;
		//���� � ��������.
		if( counter == 8 ){
			output.Write( static_cast<byte>( c ) );//������� �.
            c = 0;//�������� �.
            counter = 0;//�������� �������.
        }
	}

	//������� "�������". ����, ������� ���������� ����� � ������ �����.
	if( counter != 0 ){
		c = c << ( 8 - counter );
		output.Write( static_cast<byte>( c ) );
	}
	//������� �������� ������ � ������� ������ � ������� ������.
	for( int i = 0; i < simbolsInOrder.size(); ++i ){
		output.Write( static_cast<byte>( simbolsInOrder[i]->Key ) );
	}
	return simbols;
}

//���������� ������ �� ������ ������������ ����.
void HuffmanTree::BuildFromCode( IInputStream& input )
{
    queue<Node*> q;
    vector<Node*> simbols;
    unsigned char c;//����� ����� ��������� ��������� ����.
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

//�������� � ������� ������� ������.
void EncodeText( HuffmanTree& Tree, string& data, vector<Node*> simbols, IOutputStream& output )
{
	unsigned char c = 0;
    string s;
    short counter = 0;
	int sumOfCodes = 0;

	//������������ ����� ����, ������� ��������� ��� ��������� ������.
	for( int i = 0; i < data.size(); ++i ){
		sumOfCodes += simbols[static_cast<unsigned int>( static_cast<unsigned char>( data[i] ) )]->code.size();
    }

    //�������, ������� ��� ��������� ���� �� ������ ����� ������.
	short remain = 8 - ( sumOfCodes % 8 );

        //������ � ������ ���� ����������� ���������� ����� � �������.
        for( int i = 0; i < remain - 1; ++i ){
            ++counter;
        }

        c = ( c << 1 ) + 1;
		++counter;
		//���� � ���� ����� ����� ������, �� ������� ������� � ������ ���� 00000001.
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

//���������� �����.
void DecodeText( HuffmanTree& Tree, IInputStream& input, IOutputStream& output )
{
    unsigned char c;
    Node* currentNode = Tree.GiveRoot();
    int i = 0;
    bool b = false;
    byte value;

	//����������� ������.
	while( input.Read( value ) ){
        c = static_cast<unsigned char>( value );
        //���� ���������� ������ ��������, �� � � ������� ��� ���������� ������ �� ������ ����� ������.
        if( !b ){

            while( c >> 7 == 0 ){
                    c = c << 1;
                    ++i;
            }

            c = c << 1;
			++i;
			//���� ������ ��� ��� ��������� �������� ��������� ����, �� ���������� ����� ������� �������.
			if( i == 8 ){
				i = 0;
				input.Read( value );
				c = static_cast<unsigned char>( value );
			}
			b = true;
        }

        while( true ){
            //���� ����� �� �����.
            if( currentNode->Left == 0 ){
                output.Write( static_cast<byte>( currentNode->Key ) );
                currentNode = Tree.GiveRoot();
            }else{
                //���� ��������� �������� ��������� ����.
                if( i == 8 ){
                    i = 0;
                    break;
                }
                if( c >> 7 == 0 ){
                    currentNode = currentNode->Left;//���� ���� � ����� ���������.
                }else{
                    currentNode = currentNode->Right;//���� ���� � ������ ���������.
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
