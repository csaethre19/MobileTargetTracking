### 1. Generate SSH key pair
ssh-keygen -t rsa -b 4096 -C "your_email@example.com"

### 2. Start SSH agent and add private key
eval $(ssh-agent -s) <br>
ssh-add ~/.ssh/id_rsa

### 3. Copy 
cat ~/.ssh/id_rsa.pub

### 4. Add to GitHub account
Add the SSH key to your GitHub account: <br>
Go to your GitHub account settings.<br>
Click on "SSH and GPG keys" in the sidebar. <br>
Click on the "New SSH key" button. <br>
Paste your SSH key into the field labeled "Key". <br>
Add a descriptive title in the "Title" field, like "My laptop key" or "Work computer key". <br>
Click "Add SSH key".<br>

### 5. Clone repo
git clone git@github.com:csaethre19/MobileTargetTracking.git

