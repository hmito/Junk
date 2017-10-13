public class CardVector{
    Card[] Cards;
    int Size;
    public CardVector(){
        Cards = new Card[1];
        Size = 0;
    }
    public void push_back(Card NewCard){
        if(Cards.length == Size){
            Card[] NewCards = new Card[Size*2];
            for(int i=0;i<Cards.length;++i){
                NewCards[i] = Cards[i];
            }
            Cards = NewCards;
        }
        Cards[Size++] = NewCard;
    }
    public Card pop_back(){
        if(empty())return new Card();
        --Size;
        return Cards[Size];
    }
    public boolean empty(){return Size==0;}
    public Card back(){
        return Cards[Size-1];
    }
    public int size(){return Size;}
    public int capacity(){return Cards.length;}
    public Card at(int Pos){return Cards[Pos];}
    public String str(){
        if(empty())return "";
        String Str=Cards[0].str();
        for(int i=1;i<Size;++i){
            Str+=", "+Cards[i].str();
        }
        return Str;
    }
} 
