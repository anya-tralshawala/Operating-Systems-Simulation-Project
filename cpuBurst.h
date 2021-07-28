class cpuBurst
{
  /* data */
public:
  cpuBurst(double aCPUT, double anIOTime);
  cpuBurst();
  cpuBurst(const cpuBurst &i);
  double get_CPUtime() { return CPU_time; }
  double get_IOtime() { return IO_time; }
  void update_CPU(double time);
  bool isOriginalTime();
  double getOriginal(){return starting_CPUtime;}
private:
  double CPU_time;
  double IO_time;
  double starting_CPUtime;
};

// bool less_names();
