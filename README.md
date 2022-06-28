# Spectral bands DSP Block

Creates spectral bands between 0 and sampling_freq / 2. E.g. it creates 0.5-1.5Hz, 1.5-2.5Hz, 2.5Hz-3.5Hz etc. This is an example DSP block created for the "Advanced Anomaly Detection with Edge Impulse" video here: https://www.youtube.com/watch?v=7vr4D_zlQTE .

## How to run (free version of Edge Impulse)

Start the block:

```
PORT=6666 python3 dsp-server.py
```

Then expose this block to the internet via:

```
ngrok http 6666
```

And add the *https* URL displayed as a custom DSP block under **Create impulse**.

## How to run (enterprise version of Edge Impulse)

Initialize the block via:

```
edge-impulse-blocks init
```

Then push via:

```
edge-impulse-blocks push
```

The block is now available from the **Create impulse** screen.

## C++ implementation

Paste the implementation in [cpp/block.cpp](cpp/block.cpp) into your main.cpp.
