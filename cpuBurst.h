class cpuBurst
{
    /* data */
public:
    cpuBurst(double aCPUT, double anIOTime);
    cpuBurst();
    double get_CPUtime(){return CPU_time;}
    double get_IOtime(){return IO_time;}

    private:
      double CPU_time;
      double IO_time;


};

  // bool less_names();

