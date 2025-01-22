
import sys
import numpy as np
from scipy.signal import firwin


#source https://docs.scipy.org/doc/scipy-1.15.0/reference/generated/scipy.signal.firwin.html

window_list = [ 
    "hamming",
    "hann",
    "blackman",
    "triang",
    "nuttall",
    "exponential",
    "kaiser",
    "chebwin"
]

filter_list = [
    "lowpass",
    "highpass",
    "bandpass",
    "bandstop"
]

def calculate_coeffs():

    num_taps = int(sys.argv[1])
    sampling_freq = float(sys.argv[2])
    filter_type = sys.argv[3]
    window = sys.argv[4]
    cut_off = [float(sys.argv[5])]

    if len(sys.argv) == 7:
        cut_off.append(float(sys.argv[6]))

    if window not in window_list:
        print("window type not supported")

    elif filter_type not in filter_list:
        print("filter type not supported")

    else:
        coeffs = firwin(numtaps= num_taps, 
                        cutoff= cut_off, 
                        window= window, 
                        pass_zero= filter_type, 
                        fs= sampling_freq)
        coeffs_f32 = np.array(coeffs).astype(np.float32)

        print(coeffs_f32)

if __name__ == "__main__":
    calculate_coeffs()
    
