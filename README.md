# Kilobotics Demos

Labs: [Kilobotic Labs]{https://www.kilobotics.com/labs}

Simulator code: [Kilosim](https://github.com/jtebert/kilosim)

Experiment scripts: [kilosim-experiments](https://github.com/jtebert/kilosim-experiments)

Analysis code: [kilosim-analysis](https://github.com/jtebert/kilosim-analysis)

---

This is scripts for running experiments

## Use

- [Download Kilosim](https://github.com/jtebert/kilosim/releases) and compile the static library with `make`
- In this repository root directory, create symlinks to the compiled library and header files. For example:
  `ln -s /home/username/path/to/kilosim/src include`
  `ln -s /home/username/path/to/kilosim/bin lib`
- Compile your experiments with `make`. With no arguments, it will compile the contents of `src/` to `bin/`. You can pass a base directory (e.g., `make BASE=my_exp`) to use the `src/` and `bin/` directories within that folder.
- Run it! Paths are relative (such as used in your JSON config files). So to have them function correctly, run your resulting executable from *within* your base directory. For example, `cd my_exp` before running `./bin/experiment`
  (You can actually do `make BASE=my_exp run`, but it'll fail with relative paths for things like JSON config file)
- Clean up with `make clean` or `make BASE=my_exp clean`

