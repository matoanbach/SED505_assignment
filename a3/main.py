from collections import defaultdict
from abc import ABC, abstractmethod


'''
    Iterator declaration area start from here
'''
class SocialNetwork(ABC):
    @abstractmethod
    def createFriendsIterator(self, profileId):
        pass


class SocialMedia(SocialNetwork):
    def __init__(self):
        self.friendGraph = defaultdict(list)
        self.profiles = defaultdict(Profile)
        
    def setProfile(self, personId, personEmail):
        if personId not in self.profiles:
            self.profiles[personId] = Profile(personId, personEmail)
            
    def setFriend(self, personId, hisFriendId):
        if personId in self.profiles and hisFriendId in self.profiles:
            self.friendGraph[personId].append(self.profiles[hisFriendId])
    
    def socialListRequest(self, profileId):
        return self.friendGraph[profileId] if profileId in self.friendGraph else []
    
    def createFriendsIterator(self, profileId):
        return SocialMediaIterator(self, profileId)


class ProfileIterator(ABC):
    @abstractmethod
    def getNext(self):
        pass
    
    @abstractmethod
    def hasMore(self):
        pass


class SocialMediaIterator(ProfileIterator):
    def __init__(self, socialMedia: SocialMedia, profileId):
        self.socialMedia = socialMedia
        self.profileId = profileId
        self.currentPosition = 0
        self.cache = []
        self.initCache()
        
    def initCache(self):
        self.cache = self.socialMedia.socialListRequest(self.profileId)
        
    def getNext(self): # changes based on how data structure looks like
        if self.hasMore():
            result = self.cache[self.currentPosition]
            self.currentPosition += 1
            return result
        return None
    
    def hasMore(self):
        return self.currentPosition < len(self.cache)


class Profile:
    def __init__(self, id, email):
        self.id = id
        self.email = email

    def getId(self):
        return self.id
    
    def getEmail(self):
        return self.email


'''
    Client area will start from here
'''
class SocialGetter: 
    def get(self, iterator: SocialMediaIterator) -> list:
        info_list = []
        while iterator.hasMore():
            profile = iterator.getNext()
            info_list.append((profile.getId(), profile.getEmail()))
        
        return info_list


class Application:
    def __init__(self):
        # to request the data structure from the social network
        self.getter = SocialGetter()
        self.network = SocialMedia()
    
    def getFriendInfo(self, profile: Profile):
        iterator = self.network.createFriendsIterator(profile.getId())
        return self.getter.get(iterator)


if __name__ == "__main__":
# Example usage
    # start of the application -> calls the application interface
    app = Application()
    app.network.setProfile("1", "profile1@example.com")
    app.network.setProfile("2", "profile2@example.com")
    app.network.setProfile("3", "profile3@example.com")
    app.network.setFriend("1", "2")  # Adding friend relationship
    app.network.setFriend("1", "3")  # Adding another friend

    profile1 = app.network.profiles["1"]

    print(app.getFriendInfo(profile1))  # Should return [('2', 'profile2@example.com'), ('3', 'profile3@example.com')]
