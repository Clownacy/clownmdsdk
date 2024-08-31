export PREFIX="/opt/clownmdsdk"

# Copy the headers and scripts to the install location first,
# as they are needed for building the libraries.
cp -r headers-and-scripts/* $PREFIX

# Build the libraries.
make -C src -B

# Copy the libraries to the install location.
cp -r bin/* $PREFIX
