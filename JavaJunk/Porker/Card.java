public class Card{
    int No;
    int Suit;
    public Card(){
        No = 0;
        Suit = 0;
    }
    public Card(int No_, int Suit_){
        No = No_;
        Suit = Suit_;
    }
    public String str(){
        if(1 <= No || No <= 13){
            if(Suit==1){
                return "S"+String.valueOf(No);
            }else if(Suit==2){
                return "C"+String.valueOf(No);
            }else if(Suit==3){
                return "H"+String.valueOf(No);
            }else if(Suit==4){
                return "D"+String.valueOf(No);
            }
        }
        return "??";
    }
}