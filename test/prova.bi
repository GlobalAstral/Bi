type Human interface {
  void speak(string speech);
};

type Person struct<Human> {
  string name;
  string surname;
  int age;

  @Override
  void speak(string speech) {
    
  }
};
