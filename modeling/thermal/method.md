# Heat conduction simulation to thermocouple
The thermocouple we use is intended to measure the motor's
exhaust stream. We use an RS PRO Type K, 1.5mm diameter which
has a 310 stainless steel sheath. This has a melting point of
around 1400C, which is below the expected motor temperature

## Python setup

```
# on ubuntu:
cd modeling/thermal
python3.10 -m venv .venv
source .venv/bin/activate 

python -m pip install -r requirements.txt
```

# Nose cone heating estimation
In the simplified case, estimate the expected heat of the nose
cone at different speeds. This is to inform at which mach regime
a new (metal) nose cone is required.