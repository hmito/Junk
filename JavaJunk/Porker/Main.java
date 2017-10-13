public class Main{
    public static void main(String[] Arg){
        System.out.println("Hello World of Java!");

        CardVector Vec = new CardVector();
        Vec.push_back(new Card(1,1));
        System.out.println(Vec.str());
        System.out.println(Vec.size());
        System.out.println(Vec.capacity());
        
        Vec.push_back(new Card(2,1));
        System.out.println(Vec.str());
        System.out.println(Vec.size());
        System.out.println(Vec.capacity());

        Vec.push_back(new Card(3,1));
        System.out.println(Vec.str());
        System.out.println(Vec.size());
        System.out.println(Vec.capacity());

        Vec.push_back(new Card(4,1));
        System.out.println(Vec.str());
        System.out.println(Vec.size());
        System.out.println(Vec.capacity());

        Vec.push_back(new Card(1,2));
        System.out.println(Vec.str());
        System.out.println(Vec.size());
        System.out.println(Vec.capacity());
        
        while(!Vec.empty()){
            Vec.pop_back();            
            System.out.println(Vec.str());
            System.out.println(Vec.size());
            System.out.println(Vec.capacity());
        }
    }
}
