public class Main{
    public static void main(String[] Arg){
        System.out.println("Hello World of Java!");

        CardVector Vec = new CardVector();
        Vec.push_back(new Card(1,1));
        Vec.push_back(new Card(2,1));
        Vec.push_back(new Card(3,1));
        Vec.push_back(new Card(4,1));
        Vec.push_back(new Card(1,2));

        while(!Vec.empty()){
            System.out.println(Vec.size());
            System.out.println(Vec.back().str());
            Vec.pop_back();            
        }
        System.out.println(Vec.size());
    }
}
