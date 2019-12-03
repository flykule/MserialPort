//!
//! @file 			SerialPort.cpp
//! @author 		Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
//! @created		2014-01-07
//! @last-modified 	2017-11-27
//! @brief			The main serial port class.
//! @details
//!					See README.rst in repo root dir for more info.

// System includes
#include <androidLog.h>
#include <iostream>
#include <sstream>
#include <cstdio>    // Standard input/output definitions
#include <cstring>    // String function definitions
#include <unistd.h>    // UNIX standard function definitions
#include <fcntl.h>    // File control definitions
#include <cerrno>    // Error number definitions
#include <termios.h>    // POSIX terminal control definitions (struct termios)
#include <system_error>    // For throwing std::system_error

// User includes
#include "includes/Exception.hpp"
#include "includes/SerialPort.hpp"

namespace mn {
    namespace CppLinuxSerial {

        SerialPort::SerialPort() {
            echo_ = false;
            timeout_ms_ = defaultTimeout_ms_;
            custom_baudRate = 9600;
            readBufferSize_B_ = defaultReadBufferSize_B_;
            readBuffer_.reserve(readBufferSize_B_);
        }

        SerialPort::SerialPort(const std::string &device, int baudRate) :
                SerialPort() {
            state_ = State::CLOSED;
            device_ = device;
            custom_baudRate = baudRate;
        }

        SerialPort::~SerialPort() {
            try {
                Close();
            } catch (...) {
                // We can't do anything about this!
                // But we don't want to throw within destructor, so swallow
            }
        }

        void SerialPort::SetDevice(const std::string &device) {
            device_ = device;
            if (state_ == State::OPEN)


                ConfigureTermios();
        }

        void SerialPort::SetBaudRate(int baudRate) {
            custom_baudRate = baudRate;
            if (state_ == State::OPEN)
                ConfigureTermios();
        }

        void SerialPort::Open() {

            std::cout << "Attempting to open COM port \"" << device_ << "\"." << std::endl;

            if (device_.empty()) {
                THROW_EXCEPT("Attempted to open file when file path has not been assigned to.");
            }

            // Attempt to open file
            //this->fileDesc = open(this->filePath, O_RDWR | O_NOCTTY | O_NDELAY);

            // O_RDONLY for read-only, O_WRONLY for write only, O_RDWR for both read/write access
            // 3rd, optional parameter is mode_t mode
//            fileDesc_ = open(device_.c_str(), O_RDWR | O_NONBLOCK | O_CLOEXEC);
            fileDesc_ = open(device_.c_str(), O_RDWR |  O_NOCTTY );

            // Check status
            if (fileDesc_ == -1) {
                THROW_EXCEPT("Could not open device " + device_ +
                             ". Is the device name correct and do you have read/write permission?");
            }

            ConfigureTermios();

//            struct termios cfg;
//            tcgetattr(fileDesc_, &cfg);

//            cfmakeraw(&cfg);
//            cfsetispeed(&cfg, getBaudrate(custom_baudRate));
//            cfsetospeed(&cfg, getBaudrate(custom_baudRate));
//            this->SetTermios(cfg);

//            std::cout << "COM port opened successfully." << std::endl;
            state_ = State::OPEN;
        }

        void SerialPort::SetEcho(bool value) {
            echo_ = value;
            ConfigureTermios();
        }

        void SerialPort::ConfigureTermios() {
            std::cout << "Configuring COM port \"" << device_ << "\"." << std::endl;

            //================== CONFIGURE ==================//

            termios tty = GetTermios();
            speed_t speed;

            LOGD("input speed: %lu", (unsigned long) cfgetispeed(&tty));
            LOGD("output speed: %lu", (unsigned long) cfgetospeed(&tty));

            //================= (.c_cflag) ===============//

//            tty.c_cflag &= ~PARENB;        // No parity bit is added to the output characters
//            tty.c_cflag &= ~CSTOPB;        // Only one stop-bit is used
//            tty.c_cflag &= ~CSIZE;            // CSIZE is a mask for the number of bits per character
//            tty.c_cflag |= CS8;            // Set to 8 bits per character
//            tty.c_cflag &= ~CRTSCTS;       // Disable hadrware flow control (RTS/CTS)
//            tty.c_cflag |= CREAD |
//                           CLOCAL;                    // Turn on READ & ignore ctrl lines (CLOCAL = 1)


            //===================== (Baudrate) =================//
            cfsetispeed(&tty, getBaudrate(custom_baudRate));
            cfsetospeed(&tty, getBaudrate(custom_baudRate));

            //===================== (.c_oflag) =================//

            tty.c_oflag = 0;              // No remapping, no delays
            tty.c_oflag &= ~OPOST;            // Make raw

            //================= CONTROL CHARACTERS (.c_cc[]) ==================//

            // c_cc[VTIME] sets the inter-character timer, in units of 0.1s.
            // Only meaningful when port is set to non-canonical mode
            // VMIN = 0, VTIME = 0: No blocking, return immediately with what is available
            // VMIN > 0, VTIME = 0: read() waits for VMIN bytes, could block indefinitely
            // VMIN = 0, VTIME > 0: Block until any amount of data is available, OR timeout occurs
            // VMIN > 0, VTIME > 0: Block until either VMIN characters have been received, or VTIME
            //                      after first character has elapsed
            // c_cc[WMIN] sets the number of characters to block (wait) for when read() is called.
            // Set to 0 if you don't want read to block. Only meaningful when port set to non-canonical mode

            if (timeout_ms_ == -1) {
                // Always wait for at least one byte, this could
                // block indefinitely
                tty.c_cc[VTIME] = 0;
                tty.c_cc[VMIN] = 1;
            } else if (timeout_ms_ == 0) {
                // Setting both to 0 will give a non-blocking read
                tty.c_cc[VTIME] = 0;
                tty.c_cc[VMIN] = 0;
            } else if (timeout_ms_ > 0) {
                tty.c_cc[VTIME] = (cc_t) (timeout_ms_ / 100);    // 0.5 seconds read timeout
                tty.c_cc[VMIN] = 0;
            }


            //======================== (.c_iflag) ====================//

            tty.c_iflag &= ~(IXON | IXOFF | IXANY);            // Turn off s/w flow ctrl
            tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);



            //=========================== LOCAL MODES (c_lflag) =======================//

            // Canonical input is when read waits for EOL or EOF characters before returning. In non-canonical mode, the rate at which
            // read() returns is instead controlled by c_cc[VMIN] and c_cc[VTIME]
//            tty.c_lflag &= ~ICANON;                                // Turn off canonical input, which is suitable for pass-through
            echo_ ? (tty.c_lflag | ECHO) : (tty.c_lflag &
                                            ~(ECHO));    // Configure echo depending on echo_ boolean
            tty.c_lflag &= ~ECHOE;                                // Turn off echo erase (echo erase only relevant if canonical input is active)
            tty.c_lflag &= ~ECHONL;                                //
            tty.c_lflag &= ~ISIG;                                // Disables recognition of INTR (interrupt), QUIT and SUSP (suspend) characters



            // Try and use raw function call
            cfmakeraw(&tty);
            this->SetTermios(tty);

            /*
            // Flush port, then apply attributes
            tcflush(this->fileDesc, TCIFLUSH);

            if(tcsetattr(this->fileDesc, TCSANOW, &tty) != 0)
            {
                // Error occurred
                this->sp->PrintError(SmartPrint::Ss() << "Could not apply terminal attributes for \"" << this->filePath << "\" - " << strerror(errno));
                return;
            }*/
        }

        void SerialPort::Write(const std::string &data) {

            if (state_ != State::OPEN)
                THROW_EXCEPT(std::string() + __PRETTY_FUNCTION__ +
                             " called but state != OPEN. Please call Open() first.");

            if (fileDesc_ < 0) {
                THROW_EXCEPT(std::string() + __PRETTY_FUNCTION__ +
                             " called but file descriptor < 0, indicating file has not been opened.");
            }

            int writeResult = write(fileDesc_, data.c_str(), data.size());

            // Check status
            if (writeResult == -1) {
                throw std::system_error(EFAULT, std::system_category());
            }
        }

        void SerialPort::Read(std::string &data) {
            data.clear();

            if (fileDesc_ == 0) {
                //this->sp->PrintError(SmartPrint::Ss() << "Read() was called but file descriptor (fileDesc) was 0, indicating file has not been opened.");
                //return false;
                THROW_EXCEPT(
                        "Read() was called but file descriptor (fileDesc) was 0, indicating file has not been opened.");
            }

            // Allocate memory for read buffer
//		char buf [256];
//		memset (&buf, '\0', sizeof buf);

            // Read from file
            // We provide the underlying raw array from the readBuffer_ vector to this C api.
            // This will work because we do not delete/resize the vector while this method
            // is called
            ssize_t n = read(fileDesc_, &readBuffer_[0], readBufferSize_B_);

            // Error Handling
            if (n < 0) {
                // Read was unsuccessful
                throw std::system_error(EFAULT, std::system_category());
            }

            if (n > 0) {

//			buf[n] = '\0';
                //printf("%s\r\n", buf);
//			data.append(buf);
                data = std::string(&readBuffer_[0], n);
                //std::cout << *str << " and size of string =" << str->size() << "\r\n";
            }

            // If code reaches here, read must of been successful
        }

        termios SerialPort::GetTermios() {
            if (fileDesc_ == -1)
                throw std::runtime_error("GetTermios() called but file descriptor was not valid.");

            struct termios tty;
            memset(&tty, 0, sizeof(tty));

            // Get current settings (will be stored in termios structure)
            if (tcgetattr(fileDesc_, &tty) != 0) {
                // Error occurred
                std::cout << "Could not get terminal attributes for \"" << device_ << "\" - "
                          << strerror(errno) << std::endl;
                throw std::system_error(EFAULT, std::system_category());
                //return false;
            }

            return tty;
        }

        void SerialPort::SetTermios(termios myTermios) {
            // Flush port, then apply attributes
            usleep(10000);
            tcflush(fileDesc_, TCIFLUSH);

            if (tcsetattr(fileDesc_, TCSANOW, &myTermios) != 0) {
                // Error occurred
                std::cout << "Could not apply terminal attributes for \"" << device_ << "\" - "
                          << strerror(errno) << std::endl;
                throw std::system_error(EFAULT, std::system_category());

            }

            // Successful!
        }

        void SerialPort::Close() {
            if (fileDesc_ != -1) {
                auto retVal = close(fileDesc_);
                if (retVal != 0)
                    THROW_EXCEPT("Tried to close serial port " + device_ + ", but close() failed.");
                LOGD("串口%s已关闭", device_.c_str());
                fileDesc_ = -1;
            }

            state_ = State::CLOSED;
        }

        void SerialPort::SetTimeout(int32_t timeout_ms) {
            if (timeout_ms < -1)
                THROW_EXCEPT(std::string() + "timeout_ms provided to " + __PRETTY_FUNCTION__ +
                             " was < -1, which is invalid.");
            if (timeout_ms > 25500)
                THROW_EXCEPT(std::string() + "timeout_ms provided to " + __PRETTY_FUNCTION__ +
                             " was > 25500, which is invalid.");
            if (state_ == State::OPEN)
                THROW_EXCEPT(std::string() + __PRETTY_FUNCTION__ + " called while state == OPEN.");
            timeout_ms_ = timeout_ms;
        }

        State SerialPort::currendState() {
            return state_;
        }

        int SerialPort::getFileDescriptor() {
            return fileDesc_;
        }

        void SerialPort::Write(char *bytes, int len) {

            if (state_ != State::OPEN)
                THROW_EXCEPT(std::string() + __PRETTY_FUNCTION__ +
                             " called but state != OPEN. Please call Open() first.");

            if (fileDesc_ < 0) {
                THROW_EXCEPT(std::string() + __PRETTY_FUNCTION__ +
                             " called but file descriptor < 0, indicating file has not been opened.");
            }

            int writeResult = write(fileDesc_, bytes, static_cast<size_t>(len));
            tcdrain(fileDesc_);

            // Check status
            if (writeResult == -1) {
                throw std::system_error(EFAULT, std::system_category());
            }
        }

        SerialPort::SerialPort(const SerialPort &serialPort) {
            LOGD("开始复制,原是否开启");
        }
    } // namespace CppLinuxSerial
} // namespace mn
