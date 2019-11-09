#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <set>

using namespace std;

const unsigned long long Masks[] = {
    0x000000000000000F,
    0x00000000000000F0,
    0x0000000000000F00,
    0x000000000000F000,
    0x00000000000F0000,
    0x0000000000F00000,
    0x000000000F000000,
    0x00000000F0000000,
    0x0000000F00000000,
    0x000000F000000000,
    0x00000F0000000000,
    0x0000F00000000000,
    0x000F000000000000,
    0x00F0000000000000,
    0x0F00000000000000,
    0xF000000000000000,
};

const unsigned long long AntiMasks[] = {
    0xFFFFFFFFFFFFFFF0,
    0xFFFFFFFFFFFFFF0F,
    0xFFFFFFFFFFFFF0FF,
    0xFFFFFFFFFFFF0FFF,
    0xFFFFFFFFFFF0FFFF,
    0xFFFFFFFFFF0FFFFF,
    0xFFFFFFFFF0FFFFFF,
    0xFFFFFFFF0FFFFFFF,
    0xFFFFFFF0FFFFFFFF,
    0xFFFFFF0FFFFFFFFF,
    0xFFFFF0FFFFFFFFFF,
    0xFFFF0FFFFFFFFFFF,
    0xFFF0FFFFFFFFFFFF,
    0xFF0FFFFFFFFFFFFF,
    0xF0FFFFFFFFFFFFFF,
    0x0FFFFFFFFFFFFFFF
};

class CPosition {
public:
    explicit CPosition( const unsigned short source[] );

    // Передвижение пустышки вверх.
    CPosition Up() const;
    // Передвижение пустышки вверх.
    CPosition Down() const;
    // Передвижение пустышки вверх.
    CPosition Left() const;
    // Передвижение пустышки вверх.
    CPosition Right() const;
    void setAt( int place, unsigned char value );
    unsigned char getAt( int place ) const;
    bool IsEnd();
    unsigned short WereNull();
    unsigned long long GetData();
    vector<char> path;
    unsigned short depth;
    unsigned long long parent;

private:
    unsigned long long data; // 16 ячеек по 4 бита каждая.
    unsigned short nullPlace;
};

CPosition::CPosition( const unsigned short source[] ) :
    data( 0 ),
    nullPlace( 0 ),
    depth( 0 )
{
    for( char i = 0; i < 16; ++i ) {
        setAt( i, static_cast<unsigned char>( source[i] ) );
        if( source[i] == 0 ) {
            nullPlace = i;
        }
    }
}

// Установка значения в некоторую позицию.
void CPosition::setAt( int place, unsigned char value )
{
    data = ( data & AntiMasks[place] ) | ( static_cast<long long>( value ) << ( place << 2 ) );
}

// Получение того, что лежит в некоторой позиции.
unsigned char CPosition::getAt( int place ) const
{
    return static_cast<unsigned char>( ( data & Masks[place] ) >> ( place << 2 ) );
}

CPosition CPosition::Up() const
{
    CPosition result( *this );
    // Ставим то, что было выше, на то место, где была пустышка.
    result.setAt( nullPlace, getAt( nullPlace - 4 ) );
     // Ставим пустышку выше.
    result.setAt( nullPlace - 4, 0 );
    result.nullPlace -= 4;
    result.path = path;
    result.path.push_back( 'D' );
    result.depth = depth + 1;
    result.parent = data;
    return result;
}

CPosition CPosition::Down() const
{
    CPosition result( *this );
    // Ставим то, что было ниже, на то место, где была пустышка.
    result.setAt( nullPlace, getAt( nullPlace + 4 ) );
    // Ставим пустышку ниже.
    result.setAt( nullPlace + 4, 0 );
    result.nullPlace += 4;
    result.path = path;
    result.path.push_back( 'U' );
    result.depth = depth + 1;
    result.parent = data;
    return result;
}

CPosition CPosition::Left() const
{
    CPosition result( *this );
    // Ставим то, что было левее, на то место, где была пустышка.
    result.setAt( nullPlace, getAt( nullPlace - 1 ) );
    // Ставим пустышку левее.
    result.setAt( nullPlace - 1, 0 );
    result.nullPlace -= 1;
    result.path = path;
    result.path.push_back( 'R' );
    result.depth = depth + 1;
    result.parent = data;
    return result;
}

CPosition CPosition::Right() const
{
    CPosition result( *this );
    // Ставим то, что было правее, на то место, где была пустышка.
    result.setAt( nullPlace, getAt( nullPlace + 1 ) );
    // Ставим пустышку правее.
    result.setAt( nullPlace + 1, 0 );
    result.nullPlace += 1;
    result.path = path;
    result.path.push_back( 'L' );
    result.depth = depth + 1;
    result.parent = data;
    return result;
}

//Является ли позиция конечной.
bool CPosition::IsEnd()
{
    return data == 0x0FEDCBA987654321;
}

unsigned short CPosition::WereNull()
{
    return nullPlace;
}

unsigned long long CPosition::GetData()
{
    return data;
}

//Эвристика.
unsigned short Heuristic( const CPosition &position )
{
    unsigned short heuristic = 0;
    for( int i = 0; i < 16; ++i ) {
        unsigned char currentKnuckle = position.getAt( i );
        if( currentKnuckle != 0 ) {
            heuristic += abs( i / 4 - ( currentKnuckle - 1 ) / 4 ) + abs( i % 4 - ( currentKnuckle - 1 ) % 4 );
        }
    }
    return heuristic;
}

unsigned short boundedSearch( CPosition &position, unsigned short curEstimation, set<unsigned long long> &prevPos )
{
    unsigned short thisEstimation = position.depth + Heuristic( position );
    if( position.IsEnd() ){
        cout << position.depth << endl;
        for( int i = 0; i < position.path.size(); ++i ){
            cout << position.path[i];
        }
        return 0;
    }
    if( thisEstimation > curEstimation ){
        return thisEstimation;
    }
    unsigned short min = 200;
    unsigned short nullPos = position.WereNull();
    unsigned short sonEstimation;
    if ( nullPos % 4 != 0 ) {
        CPosition leftPosition = position.Left();
        if( leftPosition.GetData() != leftPosition.parent &&
           ( prevPos.size() > 5 || prevPos.find( leftPosition.GetData() ) ==  prevPos.end() ) ) {
            sonEstimation = boundedSearch( leftPosition, curEstimation, prevPos );
            if( !sonEstimation ){
                return 0;
            }
            prevPos.erase( leftPosition.GetData() );
            if( sonEstimation < min ){
                min = sonEstimation;
            }
        }
    }
    if ( nullPos > 3 ) {
        CPosition upPosition = position.Up();
        if( upPosition.GetData() != upPosition.parent &&
           ( prevPos.size() > 5 || prevPos.find( upPosition.GetData() ) == prevPos.end() ) ) {
            prevPos.insert( upPosition.GetData() );
            sonEstimation = boundedSearch( upPosition, curEstimation, prevPos );
            if( !sonEstimation ){
                return 0;
            }
            prevPos.erase( upPosition.GetData() );
            if( sonEstimation < min ){
                min = sonEstimation;
            }
        }
    }
    if ( nullPos % 4 != 3 ) {
        CPosition rightPosition = position.Right();
        if( rightPosition.GetData() != rightPosition.parent &&
           ( prevPos.size() > 5 || prevPos.find( rightPosition.GetData() ) == prevPos.end() ) ) {
            prevPos.insert( rightPosition.GetData() );
            sonEstimation = boundedSearch( rightPosition, curEstimation, prevPos );
            if( !sonEstimation ){
                return 0;
            }
            prevPos.erase( rightPosition.GetData() );
            if( sonEstimation < min ){
                min = sonEstimation;
            }
        }
    }
    if ( nullPos < 12 ) {
        CPosition downPosition = position.Down();
        if( downPosition.GetData() != downPosition.parent &&
           ( prevPos.size() > 5 || prevPos.find( downPosition.GetData() ) == prevPos.end() ) ) {
            prevPos.insert( downPosition.GetData() );
            sonEstimation = boundedSearch( downPosition, curEstimation, prevPos );
            if( !sonEstimation ){
                return 0;
            }
            prevPos.erase( downPosition.GetData() );
            if( sonEstimation < min ){
                min = sonEstimation;
            }
        }
    }
    return min;
}

void IDA_star( CPosition &initialPosition )
{
    unsigned short curEstimation = Heuristic( initialPosition );
    set<unsigned long long> prevPos;
    prevPos.insert( initialPosition.GetData() );
    while( true ){
        curEstimation = boundedSearch( initialPosition, curEstimation, prevPos );
        if( curEstimation == 0 ){
            return;
        }
    }
}

int main()
{
    unsigned short initalPos[16];
    for( char i = 0; i < 16; ++i ) {
        cin >> initalPos[i];
    }
    int permutations = 1;
    for( int i = 0; i < 16; ++i ) {
        if( initalPos[i] != 0 ){
            for( int j = 0; j < i; ++j ) {
                if( initalPos[j] > initalPos[i] ) {
                    permutations++;
                }
            }
        }else{
            permutations += i / 4;
        }
    }
    if( permutations % 2 == 1 ) {
        cout << -1;
        return 0;
    }
    CPosition initialPosition( initalPos );
    IDA_star( initialPosition );
    return 0;
}
