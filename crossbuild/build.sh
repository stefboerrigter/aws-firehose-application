source /usr/local/oecore-x86_64/environment-setup-cortexa8hf-vfp-neon-oe-linux-gnueabi
cmake ../
make -j4

read -p "Enter sysroot dir (default: /usr/local/oecore-x86_64):" dir
dir=${dir:-/usr/local/oecore-x86_64}

sudo cp libaws-firehose.a $dir/sysroots/cortexa8hf-vfp-neon-oe-linux-gnueabi/usr/lib/ -v
sudo cp ../FirehoseLibraryClient.h $dir/sysroots/cortexa8hf-vfp-neon-oe-linux-gnueabi/usr/include/ -v
