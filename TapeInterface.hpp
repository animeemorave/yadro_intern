#define TAPEINTERFACE_HPP_
#ifdef TAPEINTERFACE_HPP_
class TapeInterface {
public:
    virtual void read(int &value) = 0;
    virtual void write(int object) = 0;
    virtual void shift_r() = 0;
    virtual void shift_l() = 0;
    virtual void insert_value(int index) = 0;
    virtual void to_start() = 0;
    virtual bool is_eof() = 0;
};
#endif  // TAPEINTERFACE_HPP_