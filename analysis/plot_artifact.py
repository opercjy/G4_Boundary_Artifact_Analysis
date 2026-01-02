import uproot
import matplotlib.pyplot as plt
import numpy as np
import sys
import os

def analyze_root_file(filename):
    if not os.path.exists(filename):
        print(f"Error: File '{filename}' not found.")
        return

    print(f"Analyzing: {filename}")
    file = uproot.open(filename)
    
    # 1. Load Data
    steps = file["StepData"]
    z_step = steps["Z_um"].array()
    len_step = steps["StepLen_um"].array()
    
    vertex = file["VertexData"]
    z_vtx = vertex["Z_Vtx_um"].array()

    # 2. Setup Plot (2 Panels)
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 12), sharex=True)
    
    # --- Panel 1: Step Length Distribution (Check for V-Shape) ---
    ax1.scatter(z_step, len_step, s=0.5, c='black', alpha=0.3, label='Steps')
    ax1.set_title("Evidence 1: Step Length vs Position (Truncation Check)", fontsize=14, fontweight='bold')
    ax1.set_ylabel("Step Length ($\mu$m)", fontsize=12)
    ax1.axvline(0, color='red', linestyle='--', linewidth=1.5, label='Interface (Z=0)')
    ax1.grid(True, alpha=0.5, linestyle=':')
    ax1.legend(loc='upper right')
    
    # --- Panel 2: Vertex Density (Check for Dip & Pile-up) ---
    # Binning: 1 um width
    bins = np.linspace(-40, 40, 81) 
    counts, _, _ = ax2.hist(z_vtx, bins=bins, color='teal', alpha=0.7, label='Annihilation')
    
    ax2.set_title("Evidence 2: Positron Annihilation Density", fontsize=14, fontweight='bold')
    ax2.set_xlabel("Z Position ($\mu$m) [Pb < 0 | Scintillator > 0]", fontsize=12)
    ax2.set_ylabel("Counts / $\mu$m", fontsize=12)
    ax2.axvline(0, color='red', linestyle='--', linewidth=1.5)
    
    # Add Annotations (Physics Explanation)
    max_c = max(counts) if len(counts) > 0 else 1
    
    # Dense Region (Pb)
    ax2.text(-20, max_c*0.85, "DENSE (Pb)\nHigh Interaction\n(Backscatter Pile-up)", 
             ha='center', color='darkred', fontweight='bold', fontsize=10)
    
    # Sparse Region (Scint)
    ax2.text(20, max_c*0.85, "SPARSE (Scint)\nLow Interaction\n(Long Mean Free Path)", 
             ha='center', color='blue', fontweight='bold', fontsize=10)

    # Artifact Check Logic
    # Check counts in the first 3 um of Scintillator
    dip_counts = np.sum((z_vtx > 0) & (z_vtx < 3))
    ref_counts = np.mean(counts[(bins[:-1] > 5) & (bins[:-1] < 15)])
    
    if dip_counts < ref_counts * 0.2:
        ax2.annotate('ARTIFACT (DIP)', xy=(1.5, 0), xytext=(10, max_c*0.5),
                     arrowprops=dict(facecolor='red', shrink=0.05), color='red', fontweight='bold')
    else:
        ax2.annotate('PHYSICS RESTORED', xy=(1.5, ref_counts), xytext=(10, max_c*0.5),
                     arrowprops=dict(facecolor='green', shrink=0.05), color='green', fontweight='bold')

    ax2.grid(True, alpha=0.5, linestyle=':')
    
    # 3. Save
    output_name = filename.replace(".root", "_Analysis.png")
    plt.tight_layout()
    plt.savefig(output_name, dpi=300)
    print(f"Graph saved to: {output_name}")
    # plt.show() # Uncomment if running interactively

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python plot_artifact.py <root_file>")
        print("Example: python plot_artifact.py ../build/Boundary_Result.root")
    else:
        analyze_root_file(sys.argv[1])
