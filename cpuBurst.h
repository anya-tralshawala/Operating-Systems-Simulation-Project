class cpuBurst
{
  /* data */
public:
  cpuBurst(double aCPUT, double anIOTime);
  cpuBurst();
  cpuBurst(const cpuBurst &i);
  double get_CPUtime() { return CPU_time; }
  double get_IOtime() { return IO_time; }
  void update_CPU(double time) {CPU_time = 3;}
  
private:
  double CPU_time;
  double IO_time;
};

// bool less_names();
