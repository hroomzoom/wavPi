
import sys
import csv
import numpy as np
from scipy.signal import firwin

#usage python fir_coeffs num_taps rate filter window cutoff
#source https://docs.scipy.org/doc/scipy-1.15.0/reference/generated/scipy.signal.firwin.html

window_list = [ 
    "hamming",
    "hann",
    "blackman",
    "triang",
    "nuttall",
    "exponential",
    # "kaiser",
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
        
        #print coeffs and num taps
        print("NUM_TAPS ", num_taps)

        formatted_lines = []
        for i in range(0, len(coeffs_f32), 5):
            chunk = coeffs_f32[i:i+5]  
            formatted_chunk = ", ".join(f"{value:.6f}" for value in chunk) 
            formatted_lines.append(f"    {formatted_chunk}") 
        formatted_coeffs = "{\n" + ",\n".join(formatted_lines) + "\n};"
        print(formatted_coeffs)


if __name__ == "__main__":
    
    print("generating fir coeffs ...")
    calculate_coeffs()

    print("complete!")
    
