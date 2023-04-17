# Data collection planning with non-zero sensing distance for a budget and curvature constrained unmanned aerial vehicle

This repository contains the code for our paper "Data collection planning with non-zero sensing distance for a budget and curvature constrained unmanned aerial vehicle": [Penicka et al. AURO 2019](http://mrs.felk.cvut.cz/data/papers/penicka_auro_2019_vns_dopn.pdf).

[![illustration](./etc/dopn_small.jpg)](https://youtu.be/zPXZahW33-w)

## Citing

If you use this code in an academic context, please cite the following publication:

Robert Pěnička, Jan Faigl, Martin Saska and Petr Váňa. Data collection planning with non-zero sensing distance for a budget and curvature constrained unmanned aerial vehicle. Autonomous Robots, 2019. ([PDF](http://mrs.felk.cvut.cz/data/papers/penicka_auro_2019_vns_dopn.pdf))

```
@article{penicka_auro_2019_vns_dopn,
  title = {Data Collection Planning with Non-Zero Sensing Distance for a Budget and Curvature Constrained Unmanned Aerial Vehicle},
  author = {P{\v e}ni{\v c}ka, Robert and Faigl, Jan and Saska, Martin and V{\'a}{\v n}a, Petr},
  year = {2019},
  month = dec,
  journal = {Autonomous Robots},
  volume = {43},
  number = {8},
  pages = {1937--1956},
  doi = {10.1007/s10514-019-09844-5}
}
```

## License

GPL-3.0 License. Copyright (C) 2019 R. Penicka, J. Faigl, M. Saska, P. Vana (Multi-robot Systems Group and Computational Robotics Laboratory, Czech technical university in Prague).

This is a research code, expect that it changes often and any fitness for a particular purpose is disclaimed.

## Installation and usage

The code has been tested on Ubuntu 20.04.

### Downloading the code

Clone the code repository and update the submodules.<br />
`git clone https://github.com/ctu-mrs/vns_dopn.git`<br />
`cd vns_dopn`<br />
`git submodule update --init --recursive`

### Compilation and dependencies

Install following dependencies:<br />
`sudo apt-get install build-essential cmake pkg-config ccache liblog4cxx-dev`<br />

Compile the submodule jf-vendors<br />
`cd jf-vendors ; ./install.sh ; cd ..`<br />

Afterwards you can compile the code using<br />
`make`

### Running the code

After compilation you should see the main binary. The main cofiguration file is the main.cfg. To run the code simply run command:<br />
`./main`
