#!/usr/bin/env bash
set -e

echo "=========================================================="
echo "          PORTSYNC END-TO-END INTEGRATION TEST            "
echo "=========================================================="

PORT=5050
BIN="./portsync"

if [ ! -f "$BIN" ]; then
    echo "Building portsync binary..."
    make portsync
fi

# Step 1: Start C++ Server in background and capture output
echo "[E2E] Starting C++ server on port $PORT..."
$BIN --port $PORT --no-color > e2e_server.log 2>&1 &
SERVER_PID=$!

# Ensure server cleanup on exit
cleanup() {
    echo "[E2E] Cleaning up server process (PID: $SERVER_PID)..."
    kill $SERVER_PID 2>/dev/null || true
    wait $SERVER_PID 2>/dev/null || true
}
trap cleanup EXIT

# Wait for server to bind
sleep 1.5

# Step 2: Launch Python Collector in sample mode for 2 rounds
echo "[E2E] Launching Python collector (mode=sample, rounds=2)..."
PYTHONPATH=python python3 python/collector.py --port $PORT --mode sample --rounds 2 --seed 42

# Allow server time to finish writing log
sleep 1

# Step 3: Assertions on server output log
echo "[E2E] Validating C++ server output log..."

if grep -q "PORTSYNC CROSS-BROKER PORTFOLIO DASHBOARD" e2e_server.log; then
    echo "[PASS] Dashboard header found in server log."
else
    echo "[FAIL] Dashboard header missing!"
    cat e2e_server.log
    exit 1
fi

if grep -q "UNIFIED PORTFOLIO HOLDINGS" e2e_server.log; then
    echo "[PASS] Unified portfolio table present."
else
    echo "[FAIL] Unified portfolio section missing!"
    cat e2e_server.log
    exit 1
fi

if grep -q "BROKER COMPARISON SUMMARY" e2e_server.log; then
    echo "[PASS] Broker comparison summary present."
else
    echo "[FAIL] Broker comparison summary missing!"
    cat e2e_server.log
    exit 1
fi

if grep -q "1-YEAR ESTIMATED PROJECTIONS" e2e_server.log; then
    echo "[PASS] 1-Year Projections section present with honest estimate label."
else
    echo "[FAIL] Projections section missing!"
    cat e2e_server.log
    exit 1
fi

if grep -q "POLYMORPHIC VIRTUAL DISPATCH STREAM" e2e_server.log; then
    echo "[PASS] Polymorphic virtual dispatch stream present."
else
    echo "[FAIL] Polymorphic virtual dispatch stream missing!"
    cat e2e_server.log
    exit 1
fi

echo ""
echo "=========================================================="
echo "       >>> E2E INTEGRATION TEST PASSED SUCCESSFULLY <<<   "
echo "=========================================================="
rm -f e2e_server.log
